//========================================================================//
//=============================SPIFFS STUFF===============================//
//========================================================================//



FileData readFile(fs::FS &fs, const char * path) {
  logInfo("Reading file: " + String(path));
  struct FileData fd = {false, ""};
  File file = fs.open(path, FILE_READ);
  if (!file || file.isDirectory()) {
    logInfo("Failed to open file for reading: " + String(path));
    return fd;
  }


  while (file.available()) {
    char c = (char)file.read();
    fd.data += c;
  }

  file.close();
  fd.success = true;
  return fd;
}

void writeFile(fs::FS &fs, const char * path, String message) {
  logInfoSerial("Writing file: " + String(path));
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    logInfoSerial("Failed to open file for writing: " + String(path));
    return;
  }
  if (file.print(message)) {
    logInfoSerial("File written: " + String(path));
  } else {
    logInfoSerial("Write failed: " + String(path));
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, String message) {
  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    logInfoSerial("Failed to open file for appending: " + String(path));
    return;
  }
  if (!file.print(message)) {
    logInfoSerial("Append failed: " + String(path));
  }
  file.close();
}

void deleteFile(fs::FS &fs, const char * path) {
  logInfo("Deleting file: " + String(path));
  if (fs.remove(path)) {
    logInfo("- file deleted");
  } else {
    logInfo("- delete failed");
  }
}
