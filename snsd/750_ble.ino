static NimBLEServer* pServer;
static NimBLECharacteristic* pTxCharacteristic;
static bool bleConnected = false;

#define MAX_QUEUE_SIZE 10
struct CommandQueue {
    String commands[MAX_QUEUE_SIZE];
    int head = 0;
    int tail = 0;
    bool isEmpty() { return head == tail; }
    bool isFull() { return ((tail + 1) % MAX_QUEUE_SIZE) == head; }
};

static CommandQueue bleCommandQueue;

// Match Python client settings exactly
#define MAX_BLE_PACKET_SIZE 100
#define BATCH_DELIMITER "|||"
#define MAX_MESSAGE_SIZE 65536  // 64KB limit

static String incomingBatchedMessage = "";  // Buffer for receiving batched messages

void queueBLECommand(const String& command) {
    if (!bleCommandQueue.isFull()) {
        bleCommandQueue.commands[bleCommandQueue.tail] = command;
        bleCommandQueue.tail = (bleCommandQueue.tail + 1) % MAX_QUEUE_SIZE;
    }
}

String dequeueBLECommand() {
    if (!bleCommandQueue.isEmpty()) {
        String command = bleCommandQueue.commands[bleCommandQueue.head];
        bleCommandQueue.head = (bleCommandQueue.head + 1) % MAX_QUEUE_SIZE;
        return command;
    }
    return "";
}

void handleIncomingBatch(const String& batch) {
    logInfo("Raw received batch size: " + String(batch.length()) + " bytes");
    logInfo("Raw received batch content: '" + batch + "'");

    // Check message size limit
    if (incomingBatchedMessage.length() + batch.length() > MAX_MESSAGE_SIZE) {
        logInfo("Message size limit exceeded, discarding batch");
        logInfo("Current buffer size: " + String(incomingBatchedMessage.length()) + 
                ", incoming batch size: " + String(batch.length()));
        incomingBatchedMessage = "";
        return;
    }

    int delimiterPos = batch.indexOf(BATCH_DELIMITER);
    if (delimiterPos != -1) {
        // This is a continuation batch
        String batchContent = batch.substring(0, delimiterPos);
        logInfo("Found delimiter at position: " + String(delimiterPos) + 
                ", content size before delimiter: " + String(batchContent.length()));
        
        incomingBatchedMessage += batchContent;
        logInfo("Added continuation batch, buffer now (" + 
                String(incomingBatchedMessage.length()) + " bytes): '" + 
                incomingBatchedMessage + "'");
    } else {
        // This is the final batch
        incomingBatchedMessage += batch;
        logInfo("Final batch received, complete message (" + 
                String(incomingBatchedMessage.length()) + " bytes): '" + 
                incomingBatchedMessage + "'");
        queueBLECommand(incomingBatchedMessage);
        incomingBatchedMessage = "";  // Reset buffer
    }
}

class ServerCallbacks: public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) {
        bleConnected = true;
        logInfo("BLE client connected");
    }

    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) {
        bleConnected = false;
        logInfo("BLE client disconnected");
        NimBLEDevice::startAdvertising();
    }
};

// Update CharacteristicCallbacks
class CharacteristicCallbacks: public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) {
        std::string value = pCharacteristic->getValue();
        if(value.length() > 0) {
            // Log the raw received data size
            logInfo("Received raw data of size: " + String(value.length()) + " bytes");
            
            // Create String directly from data and length
            String receivedData;
            receivedData.reserve(value.length());  // Pre-allocate space
            receivedData = String(value.data(), value.length());
            
            // Log the converted data
            logInfo("Converted data size: " + String(receivedData.length()) + " bytes");
            
            handleIncomingBatch(receivedData);
        }
    }
};

String getDeviceId() {
    if(global.deviceId.length() > 0) {
        return global.deviceId;
    }
    
    // Variable to store the MAC address
    uint8_t baseMac[6];
    
    // Get the MAC address of the Bluetooth interface
    esp_read_mac(baseMac, ESP_MAC_BT);
    
    char mac[18];
    sprintf(mac, "%02x%02x%02x", baseMac[3], baseMac[4], baseMac[5]);
    
    // Create device ID
    global.deviceId = "NSD-" + String(mac);
    return global.deviceId;
}

void setupBLE() {
    String deviceId = getDeviceId();
    NimBLEDevice::init(deviceId.c_str());
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    // Create UART service
    NimBLEService* pService = pServer->createService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
    
    // Create characteristics for RX and TX
    NimBLECharacteristic* pRxCharacteristic = pService->createCharacteristic(
        "6E400002-B5A3-F393-E0A9-E50E24DCCA9E",
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
    );
    
    pTxCharacteristic = pService->createCharacteristic(
        "6E400003-B5A3-F393-E0A9-E50E24DCCA9E",
        NIMBLE_PROPERTY::NOTIFY
    );

    pRxCharacteristic->setCallbacks(new CharacteristicCallbacks());
    
    // Start the service
    pService->start();

    // Start advertising
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    
    // Make more discoverable with faster intervals
    pAdvertising->setMinInterval(32); // Minimum advertising interval (20ms * 1.6)
    pAdvertising->setMaxInterval(244); // Maximum advertising interval (150ms * 1.6)
    
    // Add service UUID
    pAdvertising->addServiceUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
    
    // Set up advertisement data
    NimBLEAdvertisementData scanResponse;
    scanResponse.setName(deviceId.c_str());
    scanResponse.setCompleteServices(NimBLEUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E"));
    pAdvertising->setScanResponseData(scanResponse);
    
    // Set up main advertisement data
    NimBLEAdvertisementData advData;
    advData.setFlags(BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP);
    advData.setName(deviceId.c_str());
    advData.setCompleteServices(NimBLEUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E"));
    pAdvertising->setAdvertisementData(advData);
    
    pAdvertising->start();
}

void stopBLE() {
    if(pServer != nullptr) {
        NimBLEDevice::deinit(true);
        pServer = nullptr;
        pTxCharacteristic = nullptr;
    }
}

bool isBLEConnected() {
    return bleConnected;
}

void sendBLEResponse(const String& message) {
    if(!bleConnected || !pTxCharacteristic) {
        logInfo("Cannot send response - not connected or no characteristic");
        return;
    }
    
    size_t messageLength = message.length();
    if (messageLength > MAX_MESSAGE_SIZE) {
        logInfo("Message too large (" + String(messageLength) + " bytes)");
        return;
    }

    size_t maxBatchSize = MAX_BLE_PACKET_SIZE - strlen(BATCH_DELIMITER);
    size_t totalBatches = (messageLength + maxBatchSize - 1) / maxBatchSize;
    
    logInfo("Sending response of " + String(messageLength) + " bytes in " + String(totalBatches) + " batches");
    logInfo("Response content: '" + message + "'");
    
    String remainingMessage = message;
    for(size_t batchNum = 0; batchNum < totalBatches; batchNum++) {
        bool isLastBatch = batchNum == totalBatches - 1;
        size_t batchSize = min(maxBatchSize, remainingMessage.length());
        
        String batch = remainingMessage.substring(0, batchSize);
        remainingMessage = remainingMessage.substring(batchSize);
        
        if (!isLastBatch) {
            batch += BATCH_DELIMITER;
        }
        
        logInfo("Sending response batch " + String(batchNum + 1) + "/" + String(totalBatches) + 
               " (size: " + String(batch.length()) + " bytes): '" + batch + "'");
        logInfo("Remaining to send: " + String(remainingMessage.length()) + " bytes");
        
        pTxCharacteristic->setValue(batch.c_str());
        if (!pTxCharacteristic->notify()) {
            logInfo("Failed to send response batch");
            return;
        }
        
        if (!isLastBatch) {
            delay(150);  // Only delay between batches
        }
    }
    
    logInfo("Response sent successfully");
} 