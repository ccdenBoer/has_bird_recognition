#include "FirmwareLoader.h"
#include <RPC.h>
#include <SdramAllocator.h>
#include <QSPIFBlockDevice.h>
#include <MBRBlockDevice.h>
#include <FATFileSystem.h>
#include "PluggableUSBMSD.h"

QSPIFBlockDevice root;
mbed::MBRBlockDevice ota_data(&root, 2);
mbed::FATFileSystem ota_data_fs("qspi");

//#define ENABLE_MASS_STORAGE

#ifdef ENABLE_MASS_STORAGE
void USBMSD::begin()
{
}

USBMSD MassStorage(&root);
#endif

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
	printf("Error mounting qspi not formatted correctly\n");
	while (true) {
	  delay(10000);
	}
  }
  printf("Qspi correctly formatted and has a size of %llu\n", ota_data.size());
  // write small file to QSPI too see if it works
  FILE *fp = fopen("/qspi/placeFwHere", "w");
  if (fp == nullptr) {
	printf("error opening file\n");
	while (true) {
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
	printf("Please copy a tfLite model onto the PORTENTA mass storage\t");
	printf("When done, please unmount the mass storage and reset the board\n");
//	   MassStorage.begin();
	while (true) {
	  delay(10000);
	}
  }
  printf("Copying tflite data to SDRAM\n");
  const std::size_t file_size = getFileSize(fw);
  // print the size of the file
  printf("File size is %d\n", file_size);

  auto buffer = Uint8Allocator.allocate(file_size);

  fread(buffer, 1, file_size, fw);

  fclose(fw);

  printf("Done copying tflite data to SDRAM\n");
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