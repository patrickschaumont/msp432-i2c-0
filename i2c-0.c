#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>

const eUSCI_I2C_MasterConfig i2cConfig = {
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        3000000,                                // SMCLK = 3MHz
        EUSCI_B_I2C_SET_DATA_RATE_400KBPS,      // Desired I2C Clock of 400khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

// This application uses to I2C peripheral on the MSP432 to connect
// to the temperature sensor.
//
// The following I2C port is used
//      UCB1  SCL  J1.11
//            SDA  J1.10
//
// UCB1 is configured as a master
//
// GPIO P6.5 UCB1 SCL 2nd peripheral
// GPIO P6.4 UCB1 SDA 2nd peripheral
//
// We scan basic data from TMP006, which has 4 device registers
// each of them 16 bit.
//
// To read from those registers, we need to do the following:
//   1. Using I2C, Write the device reg address to the TMP006 slave
//   2. Using I2C, Read two bytes from the from the TMP006 slave

// I2C address of temperature sensor
#define TMP006_SLAVE_ADDRESS    0x40

// device register addresses
#define TMP006_P_MAN_ID         0xFE
#define TMP006_P_DEVICE_ID      0xFF
#define TMP006_P_TAMBIENT       0x01
#define TMP006_P_VOBJECT        0x00

int main(void) {
  volatile unsigned i;

  WDT_A_holdTimer();

  // Configure output pins
  // GPIO P6.5 UCB1 SCL 2nd peripheral
  GPIO_setAsPeripheralModuleFunctionInputPin(
          GPIO_PORT_P6,
          GPIO_PIN5,
          GPIO_PRIMARY_MODULE_FUNCTION);

  // GPIO P6.4 UCB1 SDA 2nd peripheral
  GPIO_setAsPeripheralModuleFunctionInputPin(
          GPIO_PORT_P6,
          GPIO_PIN4,
          GPIO_PRIMARY_MODULE_FUNCTION);

  // RED LED
  GPIO_setAsOutputPin    (GPIO_PORT_P2,    GPIO_PIN6);
  GPIO_setOutputLowOnPin (GPIO_PORT_P2,    GPIO_PIN6);

  // Master Configuration
  I2C_initMaster     (EUSCI_B1_BASE, &i2cConfig);
  I2C_setSlaveAddress(EUSCI_B1_BASE, TMP006_SLAVE_ADDRESS);
  I2C_enableModule   (EUSCI_B1_BASE);

  GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN6);

  while (1) {
      GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN6);

      // Select TMP006_P_DEVICE_ID register
      I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
      I2C_masterSendSingleByte(EUSCI_B1_BASE, TMP006_P_MAN_ID);

      // Receive DEVICE ID data
      I2C_masterReceiveStart(EUSCI_B1_BASE);
      while(!(I2C_getInterruptStatus(EUSCI_B1_BASE, EUSCI_B_I2C_RECEIVE_INTERRUPT0)));
      I2C_masterReceiveMultiByteNext(EUSCI_B1_BASE);
      I2C_masterReceiveMultiByteFinish(EUSCI_B1_BASE);

      for (i=0; i<10000; i++) ;
  }

}
