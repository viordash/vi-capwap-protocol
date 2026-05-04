#pragma once

#include "IElement.h"
#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) ResultCode : ElementHeader {
    enum Type : uint32_t {
        Success = 0x00000000,
        Failure_ACListMessageElementMustBePresent = 0x01000000,
        Success_NATDetected = 0x02000000,
        JoinFailure_Unspecified = 0x03000000,
        JoinFailure_ResourceDepletion = 0x04000000,
        JoinFailure_UnknownSource = 0x05000000,
        JoinFailure_IncorrectData = 0x06000000,
        JoinFailure_SessionIdAlreadyInUse = 0x07000000,
        JoinFailure_WTPHardwareNotSupported = 0x08000000,
        JoinFailure_BindingNotSupported = 0x09000000,
        ResetFailure_UnableToReset = 0x0A000000,
        ResetFailure_FirmwareWriteError = 0x0B000000,
        ConfigurationFailure_ServiceProvided = 0x0C000000,
        ConfigurationFailure_ServiceNotProvided = 0x0D000000,
        ImageDataError_InvalidChecksum = 0x0E000000,
        ImageDataError_InvalidDataLength = 0x0F000000,
        ImageDataError_OtherError = 0x10000000,
        ImageDataError_ImageAlreadyPresent = 0x11000000,
        MessageUnexpected_InvalidInCurrentState = 0x12000000,
        MessageUnexpected_UnrecognizedRequest = 0x13000000,
        Failure_MissingMandatoryMessageElement = 0x14000000,
        Failure_UnrecognizedMessageElement = 0x15000000,
        DataTransferError = 0x16000000
    };

    Type type;

    ResultCode(const ResultCode &) = default;
    ResultCode(Type type);

    bool Validate() const;
    void Log() const;
};

struct WritableResultCode : IWritableElement {
  protected:
    ResultCode element;

  public:
    WritableResultCode(ResultCode::Type type);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableResultCode : IReadableElement {
  protected:
    ResultCode *element = nullptr;
    bool is_present = false;

  public:
    bool Deserialize(RawData *raw_data) override final;
    void Log() const override final;
    const ResultCode *Get() const;
    ElementHeader::ElementType GetElementType() const override final;
    bool IsPresent() const override final;
};