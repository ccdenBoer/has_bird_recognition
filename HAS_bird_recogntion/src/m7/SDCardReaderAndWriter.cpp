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
  printf("Reading from %s\n", fileName);
  FILE *filePointer = fopen(fileName, "r");
  if (filePointer == nullptr)
  {
	printf("Error: %s\n", strerror(errno));
    return NULL;
  }
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

void SDCardReaderAndWriter::ReadJson(char* fileName){
  char* json = ReadFileData(fileName);
  deserializeJson(doc, json);
}

void SDCardReaderAndWriter::GetModelName(char* modelName){
  ReadJson("/sd-card/config.json");
  const char* model = doc["model"];
  sprintf(modelName, "%s", model);
  printf("Model: %s", model);
  delay(5000);
}

void SDCardReaderAndWriter::GetModelData(char* modelName, char** birds, int* classes, int max_classes){
  char fileName[80];
  sprintf(fileName, "/sd-card/models/%s/%s_vogels.json", modelName, modelName);
  ReadJson(fileName);

  *classes = doc["total_birds"];
  if(*classes > max_classes){
    printf("Error: More classes than can be assigned -> increase MAX_NUMBER_OF_CLASSES\nMax: %d, from file: %d\n", max_classes, *classes);
    return;
  }

  printf("Total classes: %d\n", *classes);

  for(int i = 0; i < *classes; i++){
    const char* bird = doc["birds"][i];

    // Allocate memory for the string
    birds[i] = (char*) malloc(strlen(bird) + 1);

    if (birds[i] == NULL) {
        printf("Memory allocation failed\n");
        return;
    }

    // Copy the string to the allocated memory
    strcpy(birds[i], bird);

    printf("Added bird: %s\n", birds[i]);
  }

}

