#include "FirmwareLoader.h"
#include <RPC.h>
#include <SDRAM.h>
#include <QSPIFBlockDevice.h>
#include <MBRBlockDevice.h>
#include <FATFileSystem.h>
#include "PluggableUSBMSD.h"

QSPIFBlockDevice root;
mbed::MBRBlockDevice ota_data(&root, 2);
mbed::FATFileSystem ota_data_fs("qspi");

// void USBMSD::begin()
// {
// }

// USBMSD MassStorage(&root);


#ifdef TARGET_PORTENTA_H7_M4
#define Serial RPC
#endif

#define ALIGN_PTR(p, a)   ((p & (a-1)) ?(((uintptr_t)p + a) & ~(uintptr_t)(a-1)) : p)

long getFileSize(FILE *fp) {
  fseek(fp, 0, SEEK_END);
  int size = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  return size;
}

void waitForCorrectQspi() {
  int err = ota_data_fs.mount(&ota_data);
  if (err) {
	Serial.println("Error mounting qspi not formatted correctly");
	while (1) {
	  delay(10000);
	}
  }
  Serial.println("QSPI correctly formatted");
  // write small file to QSPI too see if it works
  FILE *fp = fopen("/qspi/placeFwHere", "w");
  if (fp == nullptr) {
	Serial.println("Error opening file");
	while (1) {
	  delay(10000);
	}
  }
  fprintf(fp, "Hello World");
  fclose(fp);
}



tfLiteModel_t tfliteToSdram() {
  //  MPU_Config();
  // Copy M4 firmware to SDRAM
  FILE *fw = fopen("/qspi/model.tflite", "r");
  if (fw == nullptr) {
	Serial.println(
		"Please copy a tfLite model onto the PORTENTA mass storage");
	Serial.println(
		"When done, please unmount the mass storage and reset the board");
	  // MassStorage.begin();
	while (1) {
	  delay(10000);
	}
  }
  Serial.println("Copying tflite data to SDRAM");
  const std::size_t file_size = getFileSize(fw);
//  print the size of the file
  Serial.print("Size of the file: ");
  Serial.println(file_size);

  auto buffer = (uint8_t *)SDRAM.malloc(file_size + 32);

  if (buffer == nullptr) {
	Serial.println("Error allocating memory");
  }
  buffer = (uint8_t *)ALIGN_PTR((uintptr_t)buffer, 32);

  fread(buffer, 1, file_size, fw);

  fclose(fw);

  Serial.println("Done copying M4 firmware to SDRAM");
  tfLiteModel_t model{};
  model.data = buffer;
  model.size = file_size;
  
  return model;
}

tfLiteModel_t loadTfliteModel() {
  waitForCorrectQspi();
  auto model = tfliteToSdram();
  return model;
}