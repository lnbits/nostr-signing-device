// Bech32 encoding table
const char *BECH32_ALPHABET = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";

// Bech32 generator values
const uint32_t BECH32_GENERATORS[] = {0x3b6a57b2, 0x26508e6d, 0x1ea119fa, 0x3d4233dd, 0x2a1462b3};

// Bech32 polymod function for calculating checksum
uint32_t polymod(const std::vector<uint8_t> &values) {
    uint32_t chk = 1;
    for (auto value : values) {
        uint8_t top = chk >> 25;
        chk = ((chk & 0x1ffffff) << 5) ^ value;
        for (uint8_t i = 0; i < 5; ++i) {
            if ((top >> i) & 1) {
                chk ^= BECH32_GENERATORS[i];
            }
        }
    }
    return chk;
}

// Expand the HRP for Bech32 checksum
std::vector<uint8_t> expandHrp(const String &hrp) {
    std::vector<uint8_t> expandedHrp;
    for (char c : hrp) {
        expandedHrp.push_back(c >> 5);
    }
    expandedHrp.push_back(0);
    for (char c : hrp) {
        expandedHrp.push_back(c & 31);
    }
    return expandedHrp;
}

// Convert hex string to byte vector
std::vector<uint8_t> hexToBytes(const String &hex) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < hex.length(); i += 2) {
        String byteString = hex.substring(i, i + 2);
        bytes.push_back((uint8_t)strtol(byteString.c_str(), nullptr, 16));
    }
    return bytes;
}

// Bech32 encoding function
String bech32Encode(const String &hrp, const std::vector<uint8_t> &data) {
    std::vector<uint8_t> expandedHrp = expandHrp(hrp);

    // Combine expanded HRP and data
    std::vector<uint8_t> combined = expandedHrp;
    combined.insert(combined.end(), data.begin(), data.end());

    // Append six zeros for checksum calculation
    for (int i = 0; i < 6; ++i) {
        combined.push_back(0);
    }

    // Compute polymod
    uint32_t checksum = polymod(combined) ^ 1;

    // Extract the 6 checksum values
    std::vector<uint8_t> checksumData(6);
    for (uint8_t i = 0; i < 6; ++i) {
        checksumData[5 - i] = checksum & 31;
        checksum >>= 5;
    }

    // Construct the final encoded string
    String encoded = hrp + "1";
    for (uint8_t d : data) {
        encoded += BECH32_ALPHABET[d];
    }
    for (uint8_t c : checksumData) {
        encoded += BECH32_ALPHABET[c];
    }

    return encoded;
}

// Convert a hex string to Nostr npub/nsec
String hexToNostr(const String &hex, const String &prefix) {
    // Decode the hex string into bytes
    std::vector<uint8_t> data = hexToBytes(hex);

    // Convert to 5-bit groups
    std::vector<uint8_t> bech32Data;
    int bits = 0;
    int value = 0;
    for (uint8_t byte : data) {
        value = (value << 8) | byte;
        bits += 8;
        while (bits >= 5) {
            bits -= 5;
            bech32Data.push_back((value >> bits) & 31);
        }
    }
    if (bits > 0) {
        bech32Data.push_back((value << (5 - bits)) & 31);
    }

    // Encode into Bech32 with the prefix
    return bech32Encode(prefix, bech32Data);
}

// Convert a Nostr npub/nsec string to a hex string
String nostrToHex(const String &bech32) {
    // Determine the prefix length
    size_t prefixEnd = bech32.indexOf('1');
    if (prefixEnd == -1) {
        return ""; // Invalid Bech32 string
    }

    // Extract data part
    String dataPart = bech32.substring(prefixEnd + 1);

    // Convert Bech32 characters to 5-bit values
    std::vector<uint8_t> fiveBitData;
    for (size_t i = 0; i < dataPart.length(); i++) {
        int index = String(BECH32_ALPHABET).indexOf(dataPart.charAt(i));
        if (index == -1) {
            return ""; // Invalid Bech32 character
        }
        fiveBitData.push_back((uint8_t)index);
    }

    // Convert 5-bit values to bytes
    std::vector<uint8_t> bytes;
    int bits = 0;
    int value = 0;
    for (uint8_t fiveBit : fiveBitData) {
        value = (value << 5) | fiveBit;
        bits += 5;
        if (bits >= 8) {
            bits -= 8;
            bytes.push_back((value >> bits) & 0xFF);
        }
    }

    // Convert byte vector to hex string
    String hex;
    for (uint8_t byte : bytes) {
        if (byte < 0x10) {
            hex += "0";
        }
        hex += String(byte, HEX);
    }

    hex.toLowerCase();
    hex = hex.substring(0, 64);
    return hex;
}