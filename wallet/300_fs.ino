//========================================================================//
//=============================SPIFFS STUFF===============================//
//========================================================================//



FileData readFile(fs::FS &fs, const char * path) {
  logSerial("Reading file:" + String(path));
  struct FileData fd = {false, ""};
  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    logSerial("- failed to open file for reading");
    return fd;
  }


  while (file.available())
    fd.data += (char)file.read();


  file.close();
  fd.success = true;
  return fd;
}

void writeFile(fs::FS &fs, const char * path, String message) {
  logSerial("Writing file: " + String(path));
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    logSerial("- failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    logSerial("- file written");
  } else {
    logSerial("- write failed");
  }
  file.close();
}

void deleteFile(fs::FS &fs, const char * path) {
  logSerial("Deleting file: " + String(path));
  if (fs.remove(path)) {
    logSerial("- file deleted");
  } else {
    logSerial("- delete failed");
  }
}
