/*******************************************************************************
 * (c) Copyright 2012 Microsemi SoC Products Group.  All rights reserved.
 *
 * SmartFusion2 MSS System Services bare metal software driver public API.
 *
 * SVN $Revision: 6082 $
 * SVN $Date: 2014-01-10 12:59:46 +0000 (Fri, 10 Jan 2014) $
 */

/*=========================================================================*//**
  @mainpage SmartFusion2 MSS System Services Bare Metal Driver.

  @section intro_sec Introduction
  The SmartFusion2 microcontroller subsystem (MSS) includes a communication
  block (COMM_BLK) allowing it to communicate with the SmartFusion2 System
  Controller. The SmartFusion2 System Controller performs a variety of system
  wide services. This software driver provides a set of functions to access
  these System Services. The driver can be adapted for use as part of an
  operating system, but the implementation of the adaptation layer between the
  driver and the operating system's driver model is outside the scope of the
  driver.
  
  @section hw_dependencies Hardware Flow Dependencies
  The MSS System Services driver does not require any configuration. It relies
  on the SmartFusion2 communication block (MSS_COMM_BLK) to communicate with the
  System Controller. The MSS_COMM_BLK is always enabled.
  The base address, register addresses and interrupt number assignment for the
  MSS_COMM_BLK are defined as constants in the SmartFusion2 CMSIS HAL. You must
  ensure that the latest SmartFusion2 CMSIS HAL is included in the project
  settings of the software tool chain used to build your project and that it is
  generated into your project.
  
  @section theory_op Theory of Operation
  The System Services driver provides access to the SmartFusion2 System
  Controller services. These system services are loosely grouped into the
  following features:
    - Reading system information
    - Cryptography
    - Non-deterministic random bit generator
    - Flash*Freeze
    - Zeroization
    - Asynchronous Messages
    - Programming Services
    - Digest Check Service
  Note: Refer to the function descriptions for further details about the
        features of each individual service.

  Reading System Information
  The System Services driver can be used to read information about the
  SmartFusion2 device and the design programmed into it using the following
  functions:
    - MSS_SYS_get_serial_number()
    - MSS_SYS_get_user_code()
    - MSS_SYS_get_design_version()
    - MSS_SYS_get_device_certificate()
    
  Cryptography Services
  The System Services driver provides cryptographic services using the following
  functions:
    - MSS_SYS_128bit_aes()
    - MSS_SYS_256bit_aes()
    - MSS_SYS_sha256()
    - MSS_SYS_hmac()
    - MSS_SYS_challenge_response()
    - MSS_SYS_key_tree()

  Non-Deterministic Random Bit Generator
  The System Services driver provides random number generation services using
  the following functions:
    - MSS_SYS_nrbg_instantiate()
    - MSS_SYS_nrbg_self_test()
    - MSS_SYS_nrbg_generate()
    - MSS_SYS_nrbg_reseed()
    - MSS_SYS_nrbg_uninstantiate()
    - MSS_SYS_nrbg_reset()
    
  Flash*Freeze
  The System Services driver can be used to request the system to enter
  Flash*Freeze mode using the following function:
    - MSS_SYS_flash_freeze()

  Zeroization
  The System Service driver can be used to destroy sensitive information on the
  SmartFusion2 device using the following function.
    - MSS_SYS_zeroize_device()
  System service driver will destroy all user configuration data, user keys,
  user security settings, NVM, SRAM, FPGA fabric, system controller memory,
  and crypto-engine registers are erased.

   Asynchronous Messages
   Asynchronous messages are sent when certain events are detected allowing the
   user design or firmware to take remedial or defensive action. No response is
   required from the user design or firmware. These messages may simply be 
   discarded if desired.
   The System Controller recalculates and compares POR digests of selected  
   components as part of its startup sequence. After completing POR Digest 
   check, system controller will send asynchronous message indicating the status
   of POR Digest check. 
   
   Programming Service
   The System services driver allows the user to reprogram the device without the 
   need for an external master using the following function.
     - MSS_SYS_start_isp()
   The In-System Programming (ISP) Service allows the MSS M3 processor to directly
   provide a bitstream for programming.  The ISP service is unique in that all 
   communication utilizes the COM channel such that the firmware receives the 
   entire bitstream as a continuous stream of bytes.
    
   Digest Check Service
   The System service driver can be used to recalculates and compares digests of
   selected components using the following function.
     - MSS_SYS_check_digest()
    
 *//*=========================================================================*/

#ifndef __MSS_SYS_SERVICES_H_
#define __MSS_SYS_SERVICES_H_ 1

#include "../../CMSIS/m2sxxx.h"
#include "mss_comblk_page_handler.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==============================================================================
 * Status codes:
 */
/*-------------------------------------------------------------------------*//**
  These constants are used by multiple services to communicate the outcome of a
  system services request. These status codes are used across all types of
  services.
  
  - MSS_SYS_SUCCESS:
      Indicates that the system services completed successfully. 
      
  - MSS_SYS_UNEXPECTED_ERROR:
      Indicates that the system failed in an unexpected way.
      
  - MSS_SYS_MEM_ACCESS_ERROR:
      Indicates that the System Controller could not access the memory used to
      pass parameters to the System Controller or to return a service result to
      the Cortex-M3.
      
  - MSS_SYS_SERVICE_DISABLED_BY_FACTORY:
      Indicates that the requested system service is not available on the
      SmartFusion2 device.
      
  - MSS_SYS_SERVICE_DISABLED_BY_USER:
      Indicates that the requested system service has been disabled as part of
      the hardware design.
  
  - MSS_SYS_SERVICE_NOT_LICENSED
      Indicates that the license is not available in SmartFusion2 device for 
      this service.
 */
#define MSS_SYS_SUCCESS                         0u
#define MSS_SYS_UNEXPECTED_ERROR                200u

#define MSS_SYS_MEM_ACCESS_ERROR                127u
#define MSS_SYS_SERVICE_NOT_LICENSED            253u
#define MSS_SYS_SERVICE_DISABLED_BY_FACTORY     254u
#define MSS_SYS_SERVICE_DISABLED_BY_USER        255u

/*-------------------------------------------------------------------------*//**
 * Programming services specific status codes:
 */
#define MSS_SYS_CHAINING_MISMATCH                   1u
#define MSS_SYS_UNEXPECTED_DATA_RECEIVED            2u
#define MSS_SYS_INVALID_ENCRYPTION_KEY              3u
#define MSS_SYS_INVALID_COMPONENT_HEADER            4u
#define MSS_SYS_BACK_LEVEL_NOT_SATISFIED            5u
#define MSS_SYS_DSN_BINDING_MISMATCH                7u
#define MSS_SYS_ILLEGAL_COMPONENT_SEQUENCE          8u
#define MSS_SYS_INSUFFICIENT_DEV_CAPABILITIES       9u
#define MSS_SYS_INCORRECT_DEVICE_ID                 10u
#define MSS_SYS_UNSUPPORTED_BITSTREAM_PROT_VER      11u
#define MSS_SYS_VERIFY_NOT_PERMITTED_ON_BITSTR      12u
#define MSS_SYS_ABORT                               127u
#define MSS_SYS_NVM_VERIFY_FAILED                   129u
#define MSS_SYS_DEVICE_SECURITY_PROTECTED           130u
#define MSS_SYS_PROGRAMMING_MODE_NOT_ENABLED        131u

/*-------------------------------------------------------------------------*//**
  These constants are used to specify the event_opcode parameter for the
  event_handler() function registered with the MSS_SYS_init() function. They are
  used to specify which asynchronous event is notified to the Cortex-M3 software
  by the System Controller. Asynchronous events are sent by the System
  Controller to the Cortex-M3 when some system events of interest occur.
  
  - FLASH_FREEZE_SHUTDOWN_OPCODE:
      Indicates that the system is being shutdown as a result of entering the
      Flash*Freeze mode.
      
  - FLASH_FREEZE_EXIT_OPCODE:
      Indicates that the system is exiting Flash*Freeze mode.
  
  - POR_DIGEST_ERROR_OPCODE
      Indicates that the MSS has received the POR Digest Check message.
 */
#define FLASH_FREEZE_SHUTDOWN_OPCODE    0xE0u
#define FLASH_FREEZE_EXIT_OPCODE        0xE1u
#define POR_DIGEST_ERROR_OPCODE         0xF1u

/*-------------------------------------------------------------------------*//**
  These constants are used to specify the options parameter for the
  MSS_SYS_flash_freeze() function.
  
  - MSS_SYS_FPGA_POWER_DOWN:
      Indicates that the MSS_SYS_flash_freeze() function should request the FPGA
      fabric to enter Flash*Freeze mode.
      
  - MSS_SYS_ENVM0_POWER_DOWN:
      Indicates that the MSS_SYS_flash_freeze() function should request eNVM0 to
      enter Flash*Freeze mode.
      
  - MSS_SYS_ENVM1_POWER_DOWN:
      Indicates that the MSS_SYS_flash_freeze() function should request eNVM1 to
      enter Flash*Freeze mode.
      
  - MSS_SYS_MPLL_POWER_DOWN:
      Indicates that the MSS_SYS_flash_freeze() function should request the MSS
      PLL to enter Flash*Freeze mode.
 */
#define MSS_SYS_FPGA_POWER_DOWN     0x00u
#define MSS_SYS_ENVM0_POWER_DOWN    0x01u
#define MSS_SYS_ENVM1_POWER_DOWN    0x02u
#define MSS_SYS_MPLL_POWER_DOWN     0x04u

/*-------------------------------------------------------------------------*//**
  These constants are used to specify the mode parameter for the
  MSS_SYS_128aes() and MSS_SYS_256bit_aes() functions.
  
  - MSS_SYS_ECB_ENCRYPT:
      Indicates that the cryptography function should perform encryption using
      the Electronic Codebook (ECB) mode.
      
  - MSS_SYS_ECB_DECRYPT:
      Indicates that the cryptography function should perform decryption using
      the Electronic Codebook (ECB) mode.
      
  - MSS_SYS_CBC_ENCRYPT:
      Indicates that the cryptography function should perform encryption using
      the Cipher-Block Chaining (CBC) mode.
      
  - MSS_SYS_CBC_DECRYPT:
      Indicates that the cryptography function should perform decryption using
      the Cipher-Block Chaining (CBC) mode.
      
  - MSS_SYS_OFB_ENCRYPT:
      Indicates that the cryptography function should perform encryption using
      the Output Feedback (OFB) mode.
      
  - MSS_SYS_OFB_DECRYPT:
      Indicates that the cryptography function should perform decryption using
      the Output Feedback (OFB) mode.
      
  - MSS_SYS_CTR_ENCRYPT:
      Indicates that the cryptography function should perform encryption using
      the Counter (CTR) mode.
      
  - MSS_SYS_CTR_DECRYPT:
      Indicates that the cryptography function should perform decryption using
      the Counter (CTR) mode.
 */
#define MSS_SYS_ECB_ENCRYPT     0x00u
#define MSS_SYS_ECB_DECRYPT     0x80u
#define MSS_SYS_CBC_ENCRYPT     0x01u
#define MSS_SYS_CBC_DECRYPT     0x81u
#define MSS_SYS_OFB_ENCRYPT     0x02u
#define MSS_SYS_OFB_DECRYPT     0x82u
#define MSS_SYS_CTR_ENCRYPT     0x03u
#define MSS_SYS_CTR_DECRYPT     0x83u

/*------------------------------------------------------------------------------
  These constants are used by non deterministic random bit generator (NDRBG)
  services to communicate the outcome of a system services request. These status
  codes are only used by NDRBG services.
  
  - MSS_SYS_NRBG_CATASTROPHIC_ERROR:
      Indicates that a catastrophic error occurred. 
      
  - MSS_SYS_NRBG_MAX_INST_EXCEEDED:
      Indicates that the maximum number of NDRBG instances has been exceeded.
      You need to release already instantiated NDRBG instances using the
      MSS_SYS_ndrbg_uninstantiate() function.
      
  - MSS_SYS_NRBG_INVALID_HANDLE:
      Indicates that the handle parameter has an invalid value.
      
  - MSS_SYS_NRBG_GEN_REQ_TOO_BIG:
      Indicates that the requested random number is too long. The requested
      length is larger than the maximum number of digits that can be generated.
      
  - MSS_SYS_NRBG_MAX_LENGTH_EXCEEDED:
      Indicates that the supplied additional data length is exceeded.
 */
#define MSS_SYS_NRBG_CATASTROPHIC_ERROR     1u
#define MSS_SYS_NRBG_MAX_INST_EXCEEDED      2u
#define MSS_SYS_NRBG_INVALID_HANDLE         3u
#define MSS_SYS_NRBG_GEN_REQ_TOO_BIG        4u
#define MSS_SYS_NRBG_MAX_LENGTH_EXCEEDED    5u

/*-------------------------------------------------------------------------*//**
  The sys_serv_isp_complete_handler_t typedef specifies the function prototype
  of in-system programming (ISP) completion handlers. This is the prototype of
  a function that is passed as parameter to the MSS_SYS_start_isp() function.
  The ISP completion handler function is called by the system services driver
  when an ISP operation initiated by a call to MSS_SYS_start_isp() completes.
  The ISP completion handler function receives a status parameter indicating
  the outcome of the ISP operation. Please refer to the MSS_SYS_start_isp()
  function description.
 */
typedef void (*sys_serv_isp_complete_handler_t)(uint32_t status);

/*-------------------------------------------------------------------------*//**
  The sys_serv_async_event_handler_t typedef specifies the function prototype of
  an asynchronous event handler that can be registered with the System Services
  driver to handle asynchronous events. This is the prototype of a function can
  be optionally implemented by the application to handle asynchronous events
  such as Flash*Freeze shutdown, Flash*Freeze exit and Asynchronous Messages.
 */
typedef void (*sys_serv_async_event_handler_t)(uint8_t event_opcode, uint8_t response);
 
/*-------------------------------------------------------------------------*//**
  This constant is used as parameter to the MSS_SYS_init() function to indicate
  that the application code does not supply an asynchronous event handler
  function.
 */
#define MSS_SYS_NO_EVENT_HANDLER    ((sys_serv_async_event_handler_t)0)

/*-------------------------------------------------------------------------*//**
  The MSS_SYS_init function initializes the system services communication with
  the System Controller.
   
  @param
    The event_handler parameter specifies an optional asynchronous event
    handler function. This event handler function is provided by the
    application. It will be called by the System Services driver whenever an
    asynchronous event is received from the SmartFusion2 System controller.
    This event handler is typically used to handle entry and exit of
    Flash*Freeze mode.
    
  @return
    This function does not return a value.
 */
void MSS_SYS_init
(
    sys_serv_async_event_handler_t event_handler
);

/*==============================================================================
 * Device and Design Information Services.
 */
 
/*-------------------------------------------------------------------------*//**
  The MSS_SYS_get_serial_number function fetches the 128-bit Device Serial
  Number (DSN).
  
  @param p_serial_number
    The p_serial_number parameter is a pointer to the 16-bytes buffer where the
    serial number will be written by this system service.
  
  @return
    The MSS_SYS_get_serial_number function returns one of following status codes:
        - MSS_SYS_SUCCESS
        - MSS_SYS_MEM_ACCESS_ERROR
        - MSS_SYS_UNEXPECTED_ERROR
 */
uint8_t MSS_SYS_get_serial_number
(
    uint8_t * p_serial_number
);

/*-------------------------------------------------------------------------*//**
  The MSS_SYS_get_user_code functions fetches the 32-bit USERCODE.
  
  @param p_user_code
    The p_user_code parameter is a pointer to the 4-bytes buffer where the
    USERCODE will be written by this system service.
  
  @return
    The MSS_SYS_get_user_code function returns one of following status codes:
        - MSS_SYS_SUCCESS
        - MSS_SYS_MEM_ACCESS_ERROR
        - MSS_SYS_UNEXPECTED_ERROR
 */
uint8_t MSS_SYS_get_user_code
(
    uint8_t * p_user_code
);

/*-------------------------------------------------------------------------*//**
  The MSS_SYS_get_design_version function fetches the design version.
  
  @param p_design_version
    The p_design_version parameter is a pointer to the 2-bytes buffer where the
    design version will be written by this system service.
  
  @return
    The MSS_SYS_get_design_version function returns one of following status codes:
        - MSS_SYS_SUCCESS
        - MSS_SYS_MEM_ACCESS_ERROR
        - MSS_SYS_UNEXPECTED_ERROR
 */
uint8_t MSS_SYS_get_design_version
(
    uint8_t * p_design_version
);

/*-------------------------------------------------------------------------*//**
  The MSS_SYS_get_device_certificate function fetches the device certificate.
  
  @param p_device_certificate
    The p_device_certificate parameter is a pointer to the 512-bytes buffer
    where the device certificate will be written by this system service.
  
  @return
    The MSS_SYS_get_device_certificate function returns one of following status
    codes:
        - MSS_SYS_SUCCESS
        - MSS_SYS_MEM_ACCESS_ERROR
        - MSS_SYS_UNEXPECTED_ERROR
 */
uint8_t MSS_SYS_get_device_certificate
(
    uint8_t * p_device_certificate
);

/*-------------------------------------------------------------------------*//**
  The MSS_SYS_flash_freeze function requests the FPGA to enter the Flash*Freeze
  mode.
  
  @param options
    The options parameter can be used to power down additional parts of
    SmartFusion2 when the FPGA fabric enters Flash*Freeze mode. This parameter
    is a bit mask of the following options:
        - MSS_SYS_FPGA_POWER_DOWN
        - MSS_SYS_ENVM0_POWER_DOWN
        - MSS_SYS_ENVM1_POWER_DOWN
        - MSS_SYS_MPLL_POWER_DOWN
    MSS_SYS_FPGA_POWER_DOWN on its own will only power down the FPGA fabric.
    MSS_SYS_ENVM0_POWER_DOWN and MSS_SYS_ENVM1_POWER_DOWN specify that eNVM
    blocks 0 and 1 respectively should enter the deep power down state during
    Flash*Freeze.
    MSS_SYS_MPLL_POWER_DOWN specifies that the MSS PLL is powered down during
    the Flash*Freeze period.
    
  @return
    The MSS_SYS_flash_freeze function returns one of following status codes:
        - MSS_SYS_SUCCESS
        - MSS_SYS_SERVICE_DISABLED_BY_FACTORY
        - MSS_SYS_SERVICE_DISABLED_BY_USER
        - MSS_SYS_UNEXPECTED_ERROR
        
  The following example demonstrates how to request the FPGA fabric and both
  eNVM0 and eNVM1 to enter the Flash*Freeze mode:
  @code
    MSS_SYS_flash_freeze(MSS_SYS_FPGA_POWER_DOWN | MSS_SYS_ENVM0_POWER_DOWN | MSS_SYS_MPLL_POWER_DOWN);
  @endcode
 */
uint8_t MSS_SYS_flash_freeze(uint8_t options);

/*==============================================================================
 * Cryptographic Services.
 */

/*-------------------------------------------------------------------------*//**
  The MSS_SYS_128bit_aes function provides access to the SmartFusion2 AES-128
  cryptography service.
  
  @param key
    The key parameter is a pointer to a 16-bytes array containing the key to use
    for the requested encryption/decryption operation.
  
  @param iv
    The iv parameter is a pointer to a 16-bytes array containing the
    initialization vector that will be used as part of the requested 
    encryption/decryption operation. Its use is different depending on the mode.
        -----------------------------------------
        | Mode |             Usage              |
        -----------------------------------------
        | ECB  | Ignored.                       |
        -----------------------------------------
        | CBC  | Randomization.                 |
        -----------------------------------------
        | OFB  | Randomization.                 |
        -----------------------------------------
        | CTR  | Used as initial counter value. |
        -----------------------------------------
  
  @param nb_blocks
    The nb_blocks parameter specifies the number of 128-bit blocks of
    plaintext/ciphertext to be processed by the AES-128 system service.
  
  @param mode
    The mode parameter specifies the cipher mode of operation and whether the
    source text must be encrypted or decrypted. The modes of operation are:
        - Electronic Codebook (ECB)
        - Cipher-Block Chaining (CBC)
        - Output Feedback (OFB)
        - Counter (CTR)
    The CTR mode uses the content of the initialization vector as its intial
    counter value. The counter increment is 2^64.
    Allowed values for the mode parameter are:
        - MSS_SYS_ECB_ENCRYPT
        - MSS_SYS_ECB_DECRYPT
        - MSS_SYS_CBC_ENCRYPT
        - MSS_SYS_CBC_DECRYPT
        - MSS_SYS_OFB_ENCRYPT
        - MSS_SYS_OFB_DECRYPT
        - MSS_SYS_CTR_ENCRYPT
        - MSS_SYS_CTR_DECRYPT
    
  @param dest_addr
    The dest_addr parameter is a pointer to the memory buffer where the result
    of the encryption/decryption operation will be stored.
  
  @param src_addr
    The src_addr parameter is a pointer to the memory buffer containing the source
    plaintext/ciphertext to be encrypted/decrypted.
  
  @return
    The MSS_SYS_128bit_aes function returns one of following status codes:
        - MSS_SYS_SUCCESS
        - MSS_SYS_MEM_ACCESS_ERROR
        - MSS_SYS_SERVICE_NOT_LICENSED
        - MSS_SYS_SERVICE_DISABLED_BY_FACTORY
        - MSS_SYS_SERVICE_DISABLED_BY_USER
 */
uint8_t MSS_SYS_128bit_aes
(
    const uint8_t * key,
    const uint8_t * iv,
    uint16_t nb_blocks,
    uint8_t mode,
    uint8_t * dest_addr,
    const uint8_t * src_addr
);

/*-------------------------------------------------------------------------*//**
  The MSS_SYS_256bit_aes function provides access to the SmartFusion2 AES-256
  cryptography service.
  
  @param key
    The key parameter is a pointer to a 32-bytes array containing the key to use
    for the requested encryption/decryption operation.
  
  @param iv
    The iv parameter is a pointer to a 16-bytes array containing the
    initialization vector that will be used as part of the requested 
    encryption/decryption operation. Its use is different depending on the mode.
        -----------------------------------------
        | Mode |             Usage              |
        -----------------------------------------
        | ECB  | Ignored.                       |
        -----------------------------------------
        | CBC  | Randomization.                 |
        -----------------------------------------
        | OFB  | Randomization.                 |
        -----------------------------------------
        | CTR  | Used as initial counter value. |
        -----------------------------------------
  
  @param nb_blocks
    The nb_blocks parameter specifies the number of 128-bit blocks of
    plaintext/ciphertext requested to be processed by the AES-128 system service.
  
  @param mode
    The mode parameter specifies the cipher mode of operation and whether the
    source text must be encrypted or decrypted. The modes of operation are:
        - Electronic Codebook (ECB)
        - Cypher-Block Chaining (CBC)
        - Output Feedback (OFB)
        - Counter (CTR)
    The CTR mode uses the content of the initialization vector as its initial
    counter value. The counter increment is 2^64.
    Allowed values for the mode parameter are:
        - MSS_SYS_ECB_ENCRYPT
        - MSS_SYS_ECB_DECRYPT
        - MSS_SYS_CBC_ENCRYPT
        - MSS_SYS_CBC_DECRYPT
        - MSS_SYS_OFB_ENCRYPT
        - MSS_SYS_OFB_DECRYPT
        - MSS_SYS_CTR_ENCRYPT
        - MSS_SYS_CTR_DECRYPT
    
  @param dest_addr
    The dest_addr parameter is a pointer to the memory buffer where the result
    of the encryption/decryption operation will be stored.
  
  @param src_addr
    The src_addr parameter is a pointer to the memory buffer containing the source
    plaintext/ciphertext to be encrypted/decrypted.
  
  @return
    The MSS_SYS_256bit_aes function returns one of following status codes:
        - MSS_SYS_SUCCESS
        - MSS_SYS_MEM_ACCESS_ERROR
        - MSS_SYS_SERVICE_NOT_LICENSED
        - MSS_SYS_SERVICE_DISABLED_BY_FACTORY
        - MSS_SYS_SERVICE_DISABLED_BY_USER
 */
uint8_t MSS_SYS_256bit_aes
( 
    const uint8_t * key,
    const uint8_t * iv,
    uint16_t nb_blocks,
    uint8_t mode,
    uint8_t * dest_addr,
    const uint8_t * src_addr
);

/*-------------------------------------------------------------------------*//**
  The MSS_SYS_sha256 function provides access to the SmartFusion2 SHA-256
  cryptography service.
  
  @param p_data_in
    The p_data_in parameter is a pointer to the memory location containing the
    data that will be hashed using the SHA-256 system service.
  
  @param length
    The length parameter specifies the length in bits of the data to hash.
  
  @param result
    The result parameter is a pointer to a 32 bytes buffer where the hash result
    will be stored.
  
  @return
    The MSS_SYS_sha256 function returns one of following status codes:
        - MSS_SYS_SUCCESS
        - MSS_SYS_MEM_ACCESS_ERROR
        - MSS_SYS_SERVICE_NOT_LICENSED
        - MSS_SYS_SERVICE_DISABLED_BY_FACTORY
        - MSS_SYS_SERVICE_DISABLED_BY_USER
 */
uint8_t MSS_SYS_sha256
(
    const uint8_t * p_data_in,
    uint32_t length,
    uint8_t * result
);

/*-------------------------------------------------------------------------*//**
  The MSS_SYS_hmac function provides access to the SmartFusion2 HMAC
  cryptography service. The HMAC system service generates message authentication
  codes using the SHA-256 hash function.
  
  @param key
    The key parameter is a pointer to a 32 bytes array containing the key used
    to generate the message authentication code.
  
  @param p_data_in
    The p_data_in parameter is a pointer to the data to be authenticated.
  
  @param length
    The length parameter specifies the number of data bytes for which to generate
    the authentication code. It is the size of the data pointed to by the
    p_data_in parameter.
  
  @param p_result
    The p_result parameter is a pointer to a 32 bytes buffer where the
    authentication code generated by the HMAC system service will be stored.
  
  @return
    The MSS_SYS_hmac function returns one of following status codes:
        - MSS_SYS_SUCCESS
        - MSS_SYS_MEM_ACCESS_ERROR
        - MSS_SYS_SERVICE_NOT_LICENSED
        - MSS_SYS_SERVICE_DISABLED_BY_FACTORY
        - MSS_SYS_SERVICE_DISABLED_BY_USER
 */
uint8_t MSS_SYS_hmac
(
    const uint8_t * key,
    const uint8_t * p_data_in,
    uint32_t length,
    uint8_t * p_result
);

/*-------------------------------------------------------------------------*//**
  The MSS_SYS_key_tree() function provides access to a SHA-256 based key-tree 
  cryptography algorithm. The key-tree service begins with a user-supplied root
  key and derives an output key based on a 7-bit parameter which can be used to
  create uniqueness for different applications using the same root key, and a
  128-bit path variable. Both the 7-bit input parameter and the 128-bit path
  variable are assumed to be publicly known. One common use for the output key
  is as a keyed validator, similar to a message authentication code tag.

  @param p_key
    The p_key parameter is a pointer to a 32 bytes array containing root key
    used to generate derived key by Key Tree service and also used to store the
    derived key.
    
  @param op_type
    The op_type parameter specifies the operational parameter to be used to
    generate the output key.
    
  @param path
    The path parameter specifies the 128-bit path to be used to generate the
    output key.
    
  @return
    The MSS_SYS_key_tree function returns one of following status codes:
        - MSS_SYS_SUCCESS
        - MSS_SYS_MEM_ACCESS_ERROR
        - MSS_SYS_SERVICE_NOT_LICENSED
        - MSS_SYS_MEM_ACCESS_ERROR
        - MSS_SYS_SERVICE_DISABLED_BY_USER
 */
uint8_t MSS_SYS_key_tree
(
    uint8_t* p_key,
    uint8_t op_type,
    const uint8_t* path
);

/*-------------------------------------------------------------------------*//**
  The MSS_SYS_challenge_response function...
  
  The MSS_SYS_challenge_response() function accept a challenge comprising a
  7-bit optype and 128-bit path and return a 256-bit response unique to the 
  given challenge and the device.

  @param p_key
    The key parameter is a pointer to a 32 bytes array containing 256 bit 
    unique response to the given challenge.

  @param op_type
    The op_type parameter specifies the operational parameter to be used to
    generate the unique 256-bits unique response.

  @param path
    The path parameter specifies the 128-bit path to be used to generate the 
    unique 256-bits unique response.

  @return
    The MSS_SYS_challenge_response function returns one of following status
    codes:
        - MSS_SYS_SUCCESS
        - MSS_SYS_MEM_ACCESS_ERROR
        - MSS_SYS_SERVICE_NOT_LICENSED
        - MSS_SYS_SERVICE_DISABLED_BY_FACTORY
        - MSS_SYS_SERVICE_DISABLED_BY_USER
 */
uint8_t MSS_SYS_challenge_response
(
    uint8_t* p_key,
    uint8_t op_type,
    const uint8_t* path
);

/*==============================================================================
 * Non Deterministic Random Bit Generator Services.
 */
 /*-------------------------------------------------------------------------*//**
   The MSS_SYS_nrbg_reset() function is used to reset the
  non-deterministic random bit generator (NRBG).

  @return
    The MSS_SYS_nrbg_self_test function returns one of following status codes:
        - MSS_SYS_SUCCESS
        - MSS_SYS_NRBG_CATASTROPHIC_ERROR
        - MSS_SYS_NRBG_MAX_INST_EXCEEDED
        - MSS_SYS_NRBG_INVALID_HANDLE
        - MSS_SYS_NRBG_GEN_REQ_TOO_BIG
        - MSS_SYS_NRBG_MAX_LENGTH_EXCEEDED
        - MSS_SYS_MEM_ACCESS_ERROR
        - MSS_SYS_SERVICE_NOT_LICENSED
        - MSS_SYS_SERVICE_DISABLED_BY_FACTORY
        - MSS_SYS_SERVICE_DISABLED_BY_USER
        - MSS_SYS_UNEXPECTED_ERROR
*/
uint8_t MSS_SYS_nrbg_reset(void);

/*-------------------------------------------------------------------------*//**
  The MSS_SYS_nrbg_self_test() function performs a self test of the
  non-deterministic random bit generator (NRBG).
  
  @return
    The MSS_SYS_nrbg_self_test function returns one of following status codes:
        - MSS_SYS_SUCCESS
        - MSS_SYS_NRBG_CATASTROPHIC_ERROR
        - MSS_SYS_NRBG_MAX_INST_EXCEEDED
        - MSS_SYS_NRBG_INVALID_HANDLE
        - MSS_SYS_NRBG_GEN_REQ_TOO_BIG
        - MSS_SYS_NRBG_MAX_LENGTH_EXCEEDED
        - MSS_SYS_MEM_ACCESS_ERROR
        - MSS_SYS_SERVICE_NOT_LICENSED
        - MSS_SYS_SERVICE_DISABLED_BY_FACTORY
        - MSS_SYS_SERVICE_DISABLED_BY_USER
        - MSS_SYS_UNEXPECTED_ERROR
 */
uint8_t MSS_SYS_nrbg_self_test(void);

/*-------------------------------------------------------------------------*//**
  The MSS_SYS_nrbg_instantiate() function instantiates a non-deterministic
  random bit generator (NRBG) instance. A maximum of two concurrent instances
  are available.
  
  @param personalization_str
    The personalization_str parameter is a pointer to a buffer containing a
    random bit generator personalization string. The personalization string
    can be up to 128 bytes long.
  
  @param personalization_str_length
    The personalization_str_length parameter specifies the byte length of the
    personalization string pointed to by personalization_str.
  
  @param p_nrbg_handle
    The p_nrbg_handle parameter is a pointer to a byte that will contain the
    handle of the instantiated NRBG if this function call succeeds.
  
  @return
    The MSS_SYS_nrbg_instantiate function returns one of following status codes:
        - MSS_SYS_SUCCESS
        - MSS_SYS_NRBG_CATASTROPHIC_ERROR
        - MSS_SYS_NRBG_MAX_INST_EXCEEDED
        - MSS_SYS_NRBG_INVALID_HANDLE
        - MSS_SYS_NRBG_GEN_REQ_TOO_BIG
        - MSS_SYS_NRBG_MAX_LENGTH_EXCEEDED
        - MSS_SYS_MEM_ACCESS_ERROR
        - MSS_SYS_SERVICE_NOT_LICENSED
        - MSS_SYS_SERVICE_DISABLED_BY_FACTORY
        - MSS_SYS_SERVICE_DISABLED_BY_USER
 */
uint8_t MSS_SYS_nrbg_instantiate
(
    const uint8_t * personalization_str,
    uint16_t personalization_str_length,
    uint8_t * p_nrbg_handle
);

/*-------------------------------------------------------------------------*//**
  The MSS_SYS_nrbg_generate function generates a random bit sequence up to
  128 bytes long.
  
  @param p_requested_data
    The p_requested_data parameter is a pointer to the buffer where the requested
    random data will be stored on completion of this system service.
  
  @param p_additional_input
    The p_additional_input parameter is a pointer to the buffer containing
    additional input data for the random bit generation.
  
  @param requested_length
    The requested_length parameter specifies the number of random data bytes
    requested to be generated. The maximum generated data length is 128 bytes.
  
  @param additional_input_length
    The additional_input_length parameter specifies the number of additional
    input bytes to use in the random data generation.
  
  @param pr_req
    The pr_req parameter specifies if prediction resistance is requested.
  
  @param nrbg_handle
    The nrbg_handle parameter specifies which non-deterministic random bit
    generator (NRBG) instance will be used to generate the random data. The
    value of nrbg_handle is obtained as a result of a previous call to the
    MSS_SYS_nrbg_instantiate() function.
  
  @return
    The MSS_SYS_nrbg_generate function returns one of following status codes:
        - MSS_SYS_SUCCESS
        - MSS_SYS_NRBG_CATASTROPHIC_ERROR
        - MSS_SYS_NRBG_MAX_INST_EXCEEDED
        - MSS_SYS_NRBG_INVALID_HANDLE
        - MSS_SYS_NRBG_GEN_REQ_TOO_BIG
        - MSS_SYS_NRBG_MAX_LENGTH_EXCEEDED
        - MSS_SYS_MEM_ACCESS_ERROR
        - MSS_SYS_SERVICE_NOT_LICENSED
        - MSS_SYS_SERVICE_DISABLED_BY_FACTORY
        - MSS_SYS_SERVICE_DISABLED_BY_USER
 */
uint8_t MSS_SYS_nrbg_generate
(
    const uint8_t * p_requested_data,
    const uint8_t * p_additional_input,
    uint8_t requested_length,
    uint8_t additional_input_length,
    uint8_t pr_req,
    uint8_t nrbg_handle
);

/*-------------------------------------------------------------------------*//**
  The MSS_SYS_nrbg_reseed() function is used to reseed the non-deterministic
  random bit generator (NRBG) identified by the nrbg_handle parameter.
  
  @param p_additional_input
    The additional_input_length parameter specifies the number of additional
    input bytes used to reseed the NRBG identified by the nrbg_handle parameter.
  
  @param additional_input_length
    The additional_input_length parameter specifies the number of additional
    input bytes used to reseed the NRBG.
  
  @param nrbg_handle
    The nrbg_handle parameter specifies which NRBG instance to reseed. The value
    of nrbg_handle is obtained as a result of a previous call to the
    MSS_SYS_nrbg_instantiate() function.
  
  @return
    The MSS_SYS_nrbg_reseed function returns one of following status codes:
        - MSS_SYS_SUCCESS
        - MSS_SYS_NRBG_CATASTROPHIC_ERROR
        - MSS_SYS_NRBG_MAX_INST_EXCEEDED
        - MSS_SYS_NRBG_INVALID_HANDLE
        - MSS_SYS_NRBG_GEN_REQ_TOO_BIG
        - MSS_SYS_NRBG_MAX_LENGTH_EXCEEDED
        - MSS_SYS_MEM_ACCESS_ERROR
        - MSS_SYS_SERVICE_NOT_LICENSED
        - MSS_SYS_SERVICE_DISABLED_BY_FACTORY
        - MSS_SYS_SERVICE_DISABLED_BY_USER
   
 */
uint8_t MSS_SYS_nrbg_reseed
(
    const uint8_t * p_additional_input,
    uint8_t additional_input_length,
    uint8_t nrbg_handle
);

/*-------------------------------------------------------------------------*//**
  The MSS_SYS_nrbg_uninstantiate() function releases the non-deterministic
  random bit generator (NRBG) identified by the nrbg_handle parameter.
  
  @param nrbg_handle
    The nrbg_handle parameter specifies which NRBG instance will be released.
    The value of nrbg_handle is obtained as a result of a previous call to the
    MSS_SYS_nrbg_instantiate() function.
  
  @return
    The MSS_SYS_nrbg_uninstantiate function returns one of following status codes:
        - MSS_SYS_SUCCESS
        - MSS_SYS_NRBG_CATASTROPHIC_ERROR
        - MSS_SYS_NRBG_MAX_INST_EXCEEDED
        - MSS_SYS_NRBG_INVALID_HANDLE
        - MSS_SYS_NRBG_GEN_REQ_TOO_BIG
        - MSS_SYS_NRBG_MAX_LENGTH_EXCEEDED
        - MSS_SYS_MEM_ACCESS_ERROR
        - MSS_SYS_SERVICE_NOT_LICENSED
        - MSS_SYS_SERVICE_DISABLED_BY_FACTORY
        - MSS_SYS_SERVICE_DISABLED_BY_USER
        - MSS_SYS_UNEXPECTED_ERROR
 */
uint8_t MSS_SYS_nrbg_uninstantiate
(
    uint8_t nrbg_handle
);

/*==============================================================================
 * Zeroization Services.
 */
/*-------------------------------------------------------------------------*//**
  The MSS_SYS_zeroize_device() function destroys sensitive information stored on
  the device. Zeroization system services driver will destroy all user configuration
  data, user keys, user security settings, NVM, SRAM, FPGA fabric, system
  controller memory, and crypto-engine registers are erased. The level of
  information destroyed by this function is configured as part of the hardware
  flow of the design programmed into the device.

  @param
    This function does not take any parameters

  @return
    This function does not return any value.
*/
void MSS_SYS_zeroize_device
(
    void
);

/*==============================================================================
 * Programming Services.
 */

#define MSS_SYS_PROG_AUTHENTICATE    0u
#define MSS_SYS_PROG_PROGRAM         1u
#define MSS_SYS_PROG_VERIFY          2u

/*-------------------------------------------------------------------------*//**
  The ISP Service allows the MSS Cortex-M3 processor to directly provide a
  bitstream for programming. The ISP Service is initiated by a call to
  MSS_SYS_start_isp(). The ISP Service can:
    - authenticate a programming bitstream
    - program a bitstream
    - verify that a programming bitstream has been correctly programmed

  The application must provide two functions as parameter to the
  MSS_SYS_start_isp() function. The first function will be used by the ISP
  Service to read the programming bitstream. The second function will be used by
  the ISP Service to notify the application that the ISP Service completed.
  
  @param mode
    The mode parameter specifies ISP service to perform. It can be one of:
        - MSS_SYS_PROG_AUTHENTICATE
        - MSS_SYS_PROG_PROGRAM
        - MSS_SYS_PROG_VERIFY
 
  @param page_read_handler
    The page_read_handler parameter is a pointer to a function with the
    following prototype:
        uint32_t page_read_handler(uint8 const ** pp_next_page);
 
  @param isp_completion_handler
    The isp_completion_handler parameter is a pointer to a function with the
    following prototype. This function will be called when the ISP service
    completes.
 
    The isp_completion_handler function will receive one of the following status
    codes:
        - MSS_SYS_SUCCESS
        - MSS_SYS_CHAINING_MISMATCH
        - MSS_SYS_UNEXPECTED_DATA_RECEIVED
        - MSS_SYS_INVALID_ENCRYPTION_KEY
        - MSS_SYS_INVALID_COMPONENT_HEADER
        - MSS_SYS_BACK_LEVEL_NOT_SATISFIED
        - MSS_SYS_DSN_BINDING_MISMATCH
        - MSS_SYS_ILLEGAL_COMPONENT_SEQUENCE
        - MSS_SYS_INSUFFICIENT_DEV_CAPABILITIES
        - MSS_SYS_INCORRECT_DEVICE_ID
        - MSS_SYS_UNSUPPORTED_BITSTREAM_PROT_VER
        - MSS_SYS_VERIFY_NOT_PERMITTED_ON_BITSTR
        - MSS_SYS_ABORT
        - MSS_SYS_NVM_VERIFY_FAILED
        - MSS_SYS_DEVICE_SECURITY_PROTECTED
        - MSS_SYS_PROGRAMMING_MODE_NOT_ENABLED
        - MSS_SYS_SERVICE_DISABLED_BY_USER
        
  @return
    This function does not return a value.
 */
void MSS_SYS_start_isp
(
    uint8_t mode,
    comblk_page_handler_t page_read_handler,
    sys_serv_isp_complete_handler_t isp_completion_handler
);

/*-------------------------------------------------------------------------*//**
  Recalculates and compares digests of selected components.
  
  @param options
    The options parameter specifies which components' digest will be recalculated
    and checked. Each bit is used to identify a component as follows:
        - bit 0: FPGA fabric
        - bit 1: eNVM0
        - bit 2: eNVM1
    Note: The FPGA fabric will enter the FlashFreeze state if powered up when
          its digest is checked.
 
  @return
    The MSS_SYS_check_digest function returns the result of the digest check. The
    meaning of the digest check return value is as follows:
        bit 0: Fabric digest error
        bit 1: ENVM0 digest error
        bit 2: ENVM1 digest error
    A '1' in one of the above bits indicates a digest mismatch.
 */
#define MSS_SYS_DIGEST_CHECK_FABRIC     0x01u
#define MSS_SYS_DIGEST_CHECK_ENVM0      0x02u
#define MSS_SYS_DIGEST_CHECK_ENVM1      0x04u

uint8_t MSS_SYS_check_digest
(
    uint8_t options
);

#ifdef __cplusplus
}
#endif

#endif /* __MSS_SYS_SERVICES_H_ */
