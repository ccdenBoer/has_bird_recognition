#include <SDCardReaderAndWriter.h>
#include <vector>

SDMMCBlockDevice blockDevice;
mbed::FATFileSystem fs("sd-card");

bool SDCardReaderAndWriter::InitSDCardReaderAndWriter()
{
  int err = fs.mount(&blockDevice);
  if (err)
  {
    // Reformat if we can't mount the filesystem
    // this should only happen on the first boot
    printf("No filesystem found, formatting... ");
    err = fs.reformat(&blockDevice); // seriously don't want to format your good data
  }
  if (err)
  {
    printf("Error formatting SDCARD: %d\n", err);
    return false;
  }
  return true;
}

void SDCardReaderAndWriter::WriteToSDCard(const char *fileName,
                                          char* dateTime,
                                          int birdType,
                                          float birdAccuracy,
                                          float lightIntensity,
                                          float temp,
                                          float hum,
                                          float rainLastHour,
                                          int batteryPercentage,
                                          float lat,
                                          float lon,
                                          uint8_t validation)
{
  // TODO: Generate name with timestamp
  // Open file or create new file if file doesn't exist

  FILE *filePointer = fopen(fileName, "w");
  if (filePointer == nullptr)
  {
	printf("Error: %s\n", strerror(errno));
    return;
  }
  printf("File created\n");

  // Create JSON object
  StaticJsonDocument<AMOUNT_OF_ITEMS_TO_WRITE> doc;
  doc["birdType"] = birdType;
  doc["date/time"] = dateTime;
  doc["birdAccuracy"] = birdAccuracy;
  doc["lightIntensity"] = lightIntensity;
  doc["temperature"] = temp;
  doc["humidity"] = hum;
  doc["rainLastHour"] = rainLastHour;
  doc["batteryPercentage"] = batteryPercentage;
  doc["lattitude"] = lat;
  doc["longtitude"] = lon;
  doc["validation"] = validation;

  printf("JSON Created\n");

  char output[1024];
  serializeJson(doc, output);

  printf("written data is: %s\n", output);

  // Write JSON object to file
  fprintf(filePointer, "%s", output);

  // Close file
  fclose(filePointer);
  printf("File closed\n");
}

long getFileSize2(FILE *fp)
{
  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  return size;
}

char buffer[1024 * 10];
char *SDCardReaderAndWriter::ReadFileData(char *fileName)
{
  FILE *filePointer = fopen(fileName, "r");
  int end = getFileSize2(filePointer);

  fread(buffer, end + 1, 1, filePointer);
  //printf("read data is: %s\n", buffer);
  fclose(filePointer);

  return buffer;
}

int SDCardReaderAndWriter::GetAmountOfFiles(char *dirName)
{
  int count = 0;
  struct dirent *entry = nullptr;
  DIR *dirPointer = opendir(dirName);

  while ((entry = readdir(dirPointer)) != NULL)
  {
    if (entry->d_type == DT_REG)
    {
      count++;
    }
  }

  rewinddir(dirPointer);

  printf("Amount of files: %d\n", count);

  return count;
}