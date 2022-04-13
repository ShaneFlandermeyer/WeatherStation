#ifndef AD1F2420_D806_4A26_8741_59AD5BC86C89
#define AD1F2420_D806_4A26_8741_59AD5BC86C89
#include <FS.h>

void appendFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

#endif /* AD1F2420_D806_4A26_8741_59AD5BC86C89 */
