#pragma once

#include "IElement.h"
#include "elements/ElementHeader.h"
#include <cstdint>

struct __attribute__((packed)) ResultCode : ElementHeader {
    enum Type : uint32_t {
        Success = ToNetworkOrder32(0),
        Failure_ACListMessageElementMustBePresent = ToNetworkOrder32(1),
        Success_NATDetected = ToNetworkOrder32(2),
        JoinFailure_Unspecified = ToNetworkOrder32(3),
        JoinFailure_ResourceDepletion = ToNetworkOrder32(4),
        JoinFailure_UnknownSource = ToNetworkOrder32(5),
        JoinFailure_IncorrectData = ToNetworkOrder32(6),
        JoinFailure_SessionIdAlreadyInUse = ToNetworkOrder32(7),
        JoinFailure_WTPHardwareNotSupported = ToNetworkOrder32(8),
        JoinFailure_BindingNotSupported = ToNetworkOrder32(9),
        ResetFailure_UnableToReset = ToNetworkOrder32(10),
        ResetFailure_FirmwareWriteError = ToNetworkOrder32(11),
        ConfigurationFailure_ServiceProvided = ToNetworkOrder32(12),
        ConfigurationFailure_ServiceNotProvided = ToNetworkOrder32(13),
        ImageDataError_InvalidChecksum = ToNetworkOrder32(14),
        ImageDataError_InvalidDataLength = ToNetworkOrder32(15),
        ImageDataError_OtherError = ToNetworkOrder32(16),
        ImageDataError_ImageAlreadyPresent = ToNetworkOrder32(17),
        MessageUnexpected_InvalidInCurrentState = ToNetworkOrder32(18),
        MessageUnexpected_UnrecognizedRequest = ToNetworkOrder32(19),
        Failure_MissingMandatoryMessageElement = ToNetworkOrder32(20),
        Failure_UnrecognizedMessageElement = ToNetworkOrder32(21),
        DataTransferError = ToNetworkOrder32(22)
    };

    Type type;

    ResultCode(const ResultCode &) = default;
    ResultCode(Type type);

    bool Validate() const;
    void Log() const;
};

struct WritableResultCode : IWritableResetResponseOptionalElement {
  protected:
    ResultCode element;

  public:
    WritableResultCode(ResultCode::Type type);

    void Serialize(RawData *raw_data) const override final;
    void Log() const override final;
};

struct ReadableResultCode : IReadableResetResponseOptionalElement {
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