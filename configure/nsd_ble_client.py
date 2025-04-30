import asyncio
import platform
import sys
import time
from bleak import BleakClient, BleakScanner

UART_SERVICE_UUID = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
UART_RX_CHAR_UUID = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"  # Write *to* device
UART_TX_CHAR_UUID = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  # Notify *from* device

# Match Arduino settings exactly
MAX_BLE_PACKET_SIZE = 100
BATCH_DELIMITER = "|||"
MAX_MESSAGE_SIZE = 65536  # 64KB limit
MAX_RETRIES = 3

# Buffer for receiving batched messages
notification_buffer = ""

def handle_rx(sender: int, data: bytearray):
    """Handle incoming BLE notifications."""
    global notification_buffer
    try:
        received = data.decode()
        
        # Process all complete batches (with delimiters)
        while BATCH_DELIMITER in received:
            delimiter_pos = received.find(BATCH_DELIMITER)
            batch = received[:delimiter_pos]
            notification_buffer += batch
            received = received[delimiter_pos + len(BATCH_DELIMITER):]
        
        # Handle any remaining data
        if received:
            notification_buffer += received
            # Check if this was the final batch (no delimiter in original data)
            if BATCH_DELIMITER not in data.decode():
                print(f"[#] {notification_buffer!r}")
                notification_buffer = ""  # Reset for next message
            
    except Exception as e:
        print(f"[ERROR] Error handling received data: {e}")
        print(f"[ERROR] Raw data length: {len(data)} bytes")
        print(f"[ERROR] Current buffer size: {len(notification_buffer)} bytes")
        notification_buffer = ""  # Reset on error

async def send_batched_command(client, command: str):
    """Send a command in batches."""
    command_bytes = command.encode()
    if len(command_bytes) > MAX_MESSAGE_SIZE:
        raise ValueError(f"Message too large ({len(command_bytes)} bytes)")

    max_batch_size = MAX_BLE_PACKET_SIZE - len(BATCH_DELIMITER)
    total_batches = (len(command_bytes) + max_batch_size - 1) // max_batch_size
    
    print(f"Sending message of {len(command_bytes)} bytes in {total_batches} batches...")
    
    # Split the message into batches
    remaining_bytes = command_bytes
    for batch_num in range(total_batches):
        is_last_batch = batch_num == total_batches - 1
        batch = remaining_bytes[:max_batch_size]
        remaining_bytes = remaining_bytes[max_batch_size:]
        
        if not is_last_batch:
            batch += BATCH_DELIMITER.encode()
            
        print(f"Sending batch {batch_num + 1}/{total_batches} (size: {len(batch)} bytes)")
        await client.write_gatt_char(UART_RX_CHAR_UUID, batch, response=True)
        if not is_last_batch:
            await asyncio.sleep(0.15)  # Only delay between batches

async def find_nsd_device() -> str:
    """Scan for 'NSD' device by name, return address or None if not found."""
    print("Scanning for 'NSD' devices...")
    timeout = 5.0 if platform.system() == "Windows" else 2.0
    devices = await BleakScanner.discover(timeout=timeout)
    for dev in devices:
        if dev.name and "NSD" in dev.name:
            print(f"  Found: {dev.name} [{dev.address}]")
            return dev.address
    return None

async def run_terminal(address: str):
    client = BleakClient(address, timeout=20.0) if platform.system() == "Windows" else BleakClient(address)
    try:
        print(f"\nAttempting to connect to {address} ...")
        await client.connect()
        await asyncio.sleep(1.0)
        print(f"Connected to {address}")

        # Subscribe
        await client.start_notify(UART_TX_CHAR_UUID, handle_rx)
        print(f"Subscribed to notifications on {UART_TX_CHAR_UUID}\n")

        print("NSD BLE Terminal\nEnter commands, or 'quit' to exit.\n")
        while True:
            command = input("> ")
            if command.lower() == "quit":
                print("Exiting...")
                break

            try:
                await send_batched_command(client, command + "")
                await asyncio.sleep(5)
            except Exception as e:
                print(f"Error sending command: {e}")

    except Exception as e:
        print(f"Connection error: {e}")
    finally:
        if client.is_connected:
            print("Disconnecting...")
            await client.disconnect()
            print("Disconnected.")

async def main():
    if platform.system() == "Windows":
        print("Initializing Bluetooth adapter...")
        time.sleep(2.0)

    # Find the NSD device
    address = None
    retries = 3
    while retries > 0 and not address:
        address = await find_nsd_device()
        if not address:
            print(f"Device not found, retrying... {retries} left")
            retries -= 1
            await asyncio.sleep(1.0)

    if not address:
        print("No NSD device found.")
        sys.exit(1)

    await run_terminal(address)

if __name__ == "__main__":
    if platform.system() == "Windows":
        asyncio.set_event_loop_policy(asyncio.WindowsSelectorEventLoopPolicy())
    asyncio.run(main())
