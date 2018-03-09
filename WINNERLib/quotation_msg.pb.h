// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: WINNERLib/quotation_msg.proto

#ifndef PROTOBUF_WINNERLib_2fquotation_5fmsg_2eproto__INCLUDED
#define PROTOBUF_WINNERLib_2fquotation_5fmsg_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message_lite.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include "TLib/core/tsystem_time.pb.h"
#include "TLib/core/tsystem_return_code.pb.h"
#include "TLib/tool/tsystem_rational_number.pb.h"
// @@protoc_insertion_point(includes)

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_WINNERLib_2fquotation_5fmsg_2eproto();
void protobuf_AssignDesc_WINNERLib_2fquotation_5fmsg_2eproto();
void protobuf_ShutdownFile_WINNERLib_2fquotation_5fmsg_2eproto();

class QuotationRequest;
class QuotationMessage;
class QuotationMessage_QuotationFillMessage;

enum QuotationReqType {
  FENBI = 0,
  ONE_MINUTE = 1,
  THREE_MINUTE = 2,
  FIVE_MINUTE = 3,
  TEN_MINUTE = 4,
  THIRTEEN_MINUTE = 5,
  HOUR = 6,
  DAY = 7
};
bool QuotationReqType_IsValid(int value);
const QuotationReqType QuotationReqType_MIN = FENBI;
const QuotationReqType QuotationReqType_MAX = DAY;
const int QuotationReqType_ARRAYSIZE = QuotationReqType_MAX + 1;

// ===================================================================

class QuotationRequest : public ::google::protobuf::MessageLite {
 public:
  QuotationRequest();
  virtual ~QuotationRequest();

  QuotationRequest(const QuotationRequest& from);

  inline QuotationRequest& operator=(const QuotationRequest& from) {
    CopyFrom(from);
    return *this;
  }

  static const QuotationRequest& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const QuotationRequest* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(QuotationRequest* other);

  // implements Message ----------------------------------------------

  QuotationRequest* New() const;
  void CheckTypeAndMergeFrom(const ::google::protobuf::MessageLite& from);
  void CopyFrom(const QuotationRequest& from);
  void MergeFrom(const QuotationRequest& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::std::string GetTypeName() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required .QuotationReqType req_type = 1;
  inline bool has_req_type() const;
  inline void clear_req_type();
  static const int kReqTypeFieldNumber = 1;
  inline ::QuotationReqType req_type() const;
  inline void set_req_type(::QuotationReqType value);

  // required string code = 2;
  inline bool has_code() const;
  inline void clear_code();
  static const int kCodeFieldNumber = 2;
  inline const ::std::string& code() const;
  inline void set_code(const ::std::string& value);
  inline void set_code(const char* value);
  inline void set_code(const char* value, size_t size);
  inline ::std::string* mutable_code();
  inline ::std::string* release_code();
  inline void set_allocated_code(::std::string* code);

  // required .Time beg_time = 3;
  inline bool has_beg_time() const;
  inline void clear_beg_time();
  static const int kBegTimeFieldNumber = 3;
  inline const ::Time& beg_time() const;
  inline ::Time* mutable_beg_time();
  inline ::Time* release_beg_time();
  inline void set_allocated_beg_time(::Time* beg_time);

  // required .Time end_time = 4;
  inline bool has_end_time() const;
  inline void clear_end_time();
  static const int kEndTimeFieldNumber = 4;
  inline const ::Time& end_time() const;
  inline ::Time* mutable_end_time();
  inline ::Time* release_end_time();
  inline void set_allocated_end_time(::Time* end_time);

  // @@protoc_insertion_point(class_scope:QuotationRequest)
 private:
  inline void set_has_req_type();
  inline void clear_has_req_type();
  inline void set_has_code();
  inline void clear_has_code();
  inline void set_has_beg_time();
  inline void clear_has_beg_time();
  inline void set_has_end_time();
  inline void clear_has_end_time();

  ::std::string* code_;
  ::Time* beg_time_;
  ::Time* end_time_;
  int req_type_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_WINNERLib_2fquotation_5fmsg_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_WINNERLib_2fquotation_5fmsg_2eproto();
  #endif
  friend void protobuf_AssignDesc_WINNERLib_2fquotation_5fmsg_2eproto();
  friend void protobuf_ShutdownFile_WINNERLib_2fquotation_5fmsg_2eproto();

  void InitAsDefaultInstance();
  static QuotationRequest* default_instance_;
};
// -------------------------------------------------------------------

class QuotationMessage_QuotationFillMessage : public ::google::protobuf::MessageLite {
 public:
  QuotationMessage_QuotationFillMessage();
  virtual ~QuotationMessage_QuotationFillMessage();

  QuotationMessage_QuotationFillMessage(const QuotationMessage_QuotationFillMessage& from);

  inline QuotationMessage_QuotationFillMessage& operator=(const QuotationMessage_QuotationFillMessage& from) {
    CopyFrom(from);
    return *this;
  }

  static const QuotationMessage_QuotationFillMessage& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const QuotationMessage_QuotationFillMessage* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(QuotationMessage_QuotationFillMessage* other);

  // implements Message ----------------------------------------------

  QuotationMessage_QuotationFillMessage* New() const;
  void CheckTypeAndMergeFrom(const ::google::protobuf::MessageLite& from);
  void CopyFrom(const QuotationMessage_QuotationFillMessage& from);
  void MergeFrom(const QuotationMessage_QuotationFillMessage& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::std::string GetTypeName() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required .Time time = 1;
  inline bool has_time() const;
  inline void clear_time();
  static const int kTimeFieldNumber = 1;
  inline const ::Time& time() const;
  inline ::Time* mutable_time();
  inline ::Time* release_time();
  inline void set_allocated_time(::Time* time);

  // required .RationalNumber price = 2;
  inline bool has_price() const;
  inline void clear_price();
  static const int kPriceFieldNumber = 2;
  inline const ::RationalNumber& price() const;
  inline ::RationalNumber* mutable_price();
  inline ::RationalNumber* release_price();
  inline void set_allocated_price(::RationalNumber* price);

  // required uint32 vol = 3;
  inline bool has_vol() const;
  inline void clear_vol();
  static const int kVolFieldNumber = 3;
  inline ::google::protobuf::uint32 vol() const;
  inline void set_vol(::google::protobuf::uint32 value);

  // optional .RationalNumber price_change = 4;
  inline bool has_price_change() const;
  inline void clear_price_change();
  static const int kPriceChangeFieldNumber = 4;
  inline const ::RationalNumber& price_change() const;
  inline ::RationalNumber* mutable_price_change();
  inline ::RationalNumber* release_price_change();
  inline void set_allocated_price_change(::RationalNumber* price_change);

  // optional bool is_change_positive = 5;
  inline bool has_is_change_positive() const;
  inline void clear_is_change_positive();
  static const int kIsChangePositiveFieldNumber = 5;
  inline bool is_change_positive() const;
  inline void set_is_change_positive(bool value);

  // @@protoc_insertion_point(class_scope:QuotationMessage.QuotationFillMessage)
 private:
  inline void set_has_time();
  inline void clear_has_time();
  inline void set_has_price();
  inline void clear_has_price();
  inline void set_has_vol();
  inline void clear_has_vol();
  inline void set_has_price_change();
  inline void clear_has_price_change();
  inline void set_has_is_change_positive();
  inline void clear_has_is_change_positive();

  ::Time* time_;
  ::RationalNumber* price_;
  ::RationalNumber* price_change_;
  ::google::protobuf::uint32 vol_;
  bool is_change_positive_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_WINNERLib_2fquotation_5fmsg_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_WINNERLib_2fquotation_5fmsg_2eproto();
  #endif
  friend void protobuf_AssignDesc_WINNERLib_2fquotation_5fmsg_2eproto();
  friend void protobuf_ShutdownFile_WINNERLib_2fquotation_5fmsg_2eproto();

  void InitAsDefaultInstance();
  static QuotationMessage_QuotationFillMessage* default_instance_;
};
// -------------------------------------------------------------------

class QuotationMessage : public ::google::protobuf::MessageLite {
 public:
  QuotationMessage();
  virtual ~QuotationMessage();

  QuotationMessage(const QuotationMessage& from);

  inline QuotationMessage& operator=(const QuotationMessage& from) {
    CopyFrom(from);
    return *this;
  }

  static const QuotationMessage& default_instance();

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  // Returns the internal default instance pointer. This function can
  // return NULL thus should not be used by the user. This is intended
  // for Protobuf internal code. Please use default_instance() declared
  // above instead.
  static inline const QuotationMessage* internal_default_instance() {
    return default_instance_;
  }
  #endif

  void Swap(QuotationMessage* other);

  // implements Message ----------------------------------------------

  QuotationMessage* New() const;
  void CheckTypeAndMergeFrom(const ::google::protobuf::MessageLite& from);
  void CopyFrom(const QuotationMessage& from);
  void MergeFrom(const QuotationMessage& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::std::string GetTypeName() const;

  // nested types ----------------------------------------------------

  typedef QuotationMessage_QuotationFillMessage QuotationFillMessage;

  // accessors -------------------------------------------------------

  // required string code = 1;
  inline bool has_code() const;
  inline void clear_code();
  static const int kCodeFieldNumber = 1;
  inline const ::std::string& code() const;
  inline void set_code(const ::std::string& value);
  inline void set_code(const char* value);
  inline void set_code(const char* value, size_t size);
  inline ::std::string* mutable_code();
  inline ::std::string* release_code();
  inline void set_allocated_code(::std::string* code);

  // repeated .QuotationMessage.QuotationFillMessage quote_fill_msg = 2;
  inline int quote_fill_msg_size() const;
  inline void clear_quote_fill_msg();
  static const int kQuoteFillMsgFieldNumber = 2;
  inline const ::QuotationMessage_QuotationFillMessage& quote_fill_msg(int index) const;
  inline ::QuotationMessage_QuotationFillMessage* mutable_quote_fill_msg(int index);
  inline ::QuotationMessage_QuotationFillMessage* add_quote_fill_msg();
  inline const ::google::protobuf::RepeatedPtrField< ::QuotationMessage_QuotationFillMessage >&
      quote_fill_msg() const;
  inline ::google::protobuf::RepeatedPtrField< ::QuotationMessage_QuotationFillMessage >*
      mutable_quote_fill_msg();

  // @@protoc_insertion_point(class_scope:QuotationMessage)
 private:
  inline void set_has_code();
  inline void clear_has_code();

  ::std::string* code_;
  ::google::protobuf::RepeatedPtrField< ::QuotationMessage_QuotationFillMessage > quote_fill_msg_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  #ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  friend void  protobuf_AddDesc_WINNERLib_2fquotation_5fmsg_2eproto_impl();
  #else
  friend void  protobuf_AddDesc_WINNERLib_2fquotation_5fmsg_2eproto();
  #endif
  friend void protobuf_AssignDesc_WINNERLib_2fquotation_5fmsg_2eproto();
  friend void protobuf_ShutdownFile_WINNERLib_2fquotation_5fmsg_2eproto();

  void InitAsDefaultInstance();
  static QuotationMessage* default_instance_;
};
// ===================================================================


// ===================================================================

// QuotationRequest

// required .QuotationReqType req_type = 1;
inline bool QuotationRequest::has_req_type() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void QuotationRequest::set_has_req_type() {
  _has_bits_[0] |= 0x00000001u;
}
inline void QuotationRequest::clear_has_req_type() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void QuotationRequest::clear_req_type() {
  req_type_ = 0;
  clear_has_req_type();
}
inline ::QuotationReqType QuotationRequest::req_type() const {
  return static_cast< ::QuotationReqType >(req_type_);
}
inline void QuotationRequest::set_req_type(::QuotationReqType value) {
  assert(::QuotationReqType_IsValid(value));
  set_has_req_type();
  req_type_ = value;
}

// required string code = 2;
inline bool QuotationRequest::has_code() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void QuotationRequest::set_has_code() {
  _has_bits_[0] |= 0x00000002u;
}
inline void QuotationRequest::clear_has_code() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void QuotationRequest::clear_code() {
  if (code_ != &::google::protobuf::internal::kEmptyString) {
    code_->clear();
  }
  clear_has_code();
}
inline const ::std::string& QuotationRequest::code() const {
  return *code_;
}
inline void QuotationRequest::set_code(const ::std::string& value) {
  set_has_code();
  if (code_ == &::google::protobuf::internal::kEmptyString) {
    code_ = new ::std::string;
  }
  code_->assign(value);
}
inline void QuotationRequest::set_code(const char* value) {
  set_has_code();
  if (code_ == &::google::protobuf::internal::kEmptyString) {
    code_ = new ::std::string;
  }
  code_->assign(value);
}
inline void QuotationRequest::set_code(const char* value, size_t size) {
  set_has_code();
  if (code_ == &::google::protobuf::internal::kEmptyString) {
    code_ = new ::std::string;
  }
  code_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* QuotationRequest::mutable_code() {
  set_has_code();
  if (code_ == &::google::protobuf::internal::kEmptyString) {
    code_ = new ::std::string;
  }
  return code_;
}
inline ::std::string* QuotationRequest::release_code() {
  clear_has_code();
  if (code_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = code_;
    code_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void QuotationRequest::set_allocated_code(::std::string* code) {
  if (code_ != &::google::protobuf::internal::kEmptyString) {
    delete code_;
  }
  if (code) {
    set_has_code();
    code_ = code;
  } else {
    clear_has_code();
    code_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// required .Time beg_time = 3;
inline bool QuotationRequest::has_beg_time() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void QuotationRequest::set_has_beg_time() {
  _has_bits_[0] |= 0x00000004u;
}
inline void QuotationRequest::clear_has_beg_time() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void QuotationRequest::clear_beg_time() {
  if (beg_time_ != NULL) beg_time_->::Time::Clear();
  clear_has_beg_time();
}
inline const ::Time& QuotationRequest::beg_time() const {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  return beg_time_ != NULL ? *beg_time_ : *default_instance().beg_time_;
#else
  return beg_time_ != NULL ? *beg_time_ : *default_instance_->beg_time_;
#endif
}
inline ::Time* QuotationRequest::mutable_beg_time() {
  set_has_beg_time();
  if (beg_time_ == NULL) beg_time_ = new ::Time;
  return beg_time_;
}
inline ::Time* QuotationRequest::release_beg_time() {
  clear_has_beg_time();
  ::Time* temp = beg_time_;
  beg_time_ = NULL;
  return temp;
}
inline void QuotationRequest::set_allocated_beg_time(::Time* beg_time) {
  delete beg_time_;
  beg_time_ = beg_time;
  if (beg_time) {
    set_has_beg_time();
  } else {
    clear_has_beg_time();
  }
}

// required .Time end_time = 4;
inline bool QuotationRequest::has_end_time() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void QuotationRequest::set_has_end_time() {
  _has_bits_[0] |= 0x00000008u;
}
inline void QuotationRequest::clear_has_end_time() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void QuotationRequest::clear_end_time() {
  if (end_time_ != NULL) end_time_->::Time::Clear();
  clear_has_end_time();
}
inline const ::Time& QuotationRequest::end_time() const {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  return end_time_ != NULL ? *end_time_ : *default_instance().end_time_;
#else
  return end_time_ != NULL ? *end_time_ : *default_instance_->end_time_;
#endif
}
inline ::Time* QuotationRequest::mutable_end_time() {
  set_has_end_time();
  if (end_time_ == NULL) end_time_ = new ::Time;
  return end_time_;
}
inline ::Time* QuotationRequest::release_end_time() {
  clear_has_end_time();
  ::Time* temp = end_time_;
  end_time_ = NULL;
  return temp;
}
inline void QuotationRequest::set_allocated_end_time(::Time* end_time) {
  delete end_time_;
  end_time_ = end_time;
  if (end_time) {
    set_has_end_time();
  } else {
    clear_has_end_time();
  }
}

// -------------------------------------------------------------------

// QuotationMessage_QuotationFillMessage

// required .Time time = 1;
inline bool QuotationMessage_QuotationFillMessage::has_time() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void QuotationMessage_QuotationFillMessage::set_has_time() {
  _has_bits_[0] |= 0x00000001u;
}
inline void QuotationMessage_QuotationFillMessage::clear_has_time() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void QuotationMessage_QuotationFillMessage::clear_time() {
  if (time_ != NULL) time_->::Time::Clear();
  clear_has_time();
}
inline const ::Time& QuotationMessage_QuotationFillMessage::time() const {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  return time_ != NULL ? *time_ : *default_instance().time_;
#else
  return time_ != NULL ? *time_ : *default_instance_->time_;
#endif
}
inline ::Time* QuotationMessage_QuotationFillMessage::mutable_time() {
  set_has_time();
  if (time_ == NULL) time_ = new ::Time;
  return time_;
}
inline ::Time* QuotationMessage_QuotationFillMessage::release_time() {
  clear_has_time();
  ::Time* temp = time_;
  time_ = NULL;
  return temp;
}
inline void QuotationMessage_QuotationFillMessage::set_allocated_time(::Time* time) {
  delete time_;
  time_ = time;
  if (time) {
    set_has_time();
  } else {
    clear_has_time();
  }
}

// required .RationalNumber price = 2;
inline bool QuotationMessage_QuotationFillMessage::has_price() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void QuotationMessage_QuotationFillMessage::set_has_price() {
  _has_bits_[0] |= 0x00000002u;
}
inline void QuotationMessage_QuotationFillMessage::clear_has_price() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void QuotationMessage_QuotationFillMessage::clear_price() {
  if (price_ != NULL) price_->::RationalNumber::Clear();
  clear_has_price();
}
inline const ::RationalNumber& QuotationMessage_QuotationFillMessage::price() const {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  return price_ != NULL ? *price_ : *default_instance().price_;
#else
  return price_ != NULL ? *price_ : *default_instance_->price_;
#endif
}
inline ::RationalNumber* QuotationMessage_QuotationFillMessage::mutable_price() {
  set_has_price();
  if (price_ == NULL) price_ = new ::RationalNumber;
  return price_;
}
inline ::RationalNumber* QuotationMessage_QuotationFillMessage::release_price() {
  clear_has_price();
  ::RationalNumber* temp = price_;
  price_ = NULL;
  return temp;
}
inline void QuotationMessage_QuotationFillMessage::set_allocated_price(::RationalNumber* price) {
  delete price_;
  price_ = price;
  if (price) {
    set_has_price();
  } else {
    clear_has_price();
  }
}

// required uint32 vol = 3;
inline bool QuotationMessage_QuotationFillMessage::has_vol() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void QuotationMessage_QuotationFillMessage::set_has_vol() {
  _has_bits_[0] |= 0x00000004u;
}
inline void QuotationMessage_QuotationFillMessage::clear_has_vol() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void QuotationMessage_QuotationFillMessage::clear_vol() {
  vol_ = 0u;
  clear_has_vol();
}
inline ::google::protobuf::uint32 QuotationMessage_QuotationFillMessage::vol() const {
  return vol_;
}
inline void QuotationMessage_QuotationFillMessage::set_vol(::google::protobuf::uint32 value) {
  set_has_vol();
  vol_ = value;
}

// optional .RationalNumber price_change = 4;
inline bool QuotationMessage_QuotationFillMessage::has_price_change() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void QuotationMessage_QuotationFillMessage::set_has_price_change() {
  _has_bits_[0] |= 0x00000008u;
}
inline void QuotationMessage_QuotationFillMessage::clear_has_price_change() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void QuotationMessage_QuotationFillMessage::clear_price_change() {
  if (price_change_ != NULL) price_change_->::RationalNumber::Clear();
  clear_has_price_change();
}
inline const ::RationalNumber& QuotationMessage_QuotationFillMessage::price_change() const {
#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
  return price_change_ != NULL ? *price_change_ : *default_instance().price_change_;
#else
  return price_change_ != NULL ? *price_change_ : *default_instance_->price_change_;
#endif
}
inline ::RationalNumber* QuotationMessage_QuotationFillMessage::mutable_price_change() {
  set_has_price_change();
  if (price_change_ == NULL) price_change_ = new ::RationalNumber;
  return price_change_;
}
inline ::RationalNumber* QuotationMessage_QuotationFillMessage::release_price_change() {
  clear_has_price_change();
  ::RationalNumber* temp = price_change_;
  price_change_ = NULL;
  return temp;
}
inline void QuotationMessage_QuotationFillMessage::set_allocated_price_change(::RationalNumber* price_change) {
  delete price_change_;
  price_change_ = price_change;
  if (price_change) {
    set_has_price_change();
  } else {
    clear_has_price_change();
  }
}

// optional bool is_change_positive = 5;
inline bool QuotationMessage_QuotationFillMessage::has_is_change_positive() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void QuotationMessage_QuotationFillMessage::set_has_is_change_positive() {
  _has_bits_[0] |= 0x00000010u;
}
inline void QuotationMessage_QuotationFillMessage::clear_has_is_change_positive() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void QuotationMessage_QuotationFillMessage::clear_is_change_positive() {
  is_change_positive_ = false;
  clear_has_is_change_positive();
}
inline bool QuotationMessage_QuotationFillMessage::is_change_positive() const {
  return is_change_positive_;
}
inline void QuotationMessage_QuotationFillMessage::set_is_change_positive(bool value) {
  set_has_is_change_positive();
  is_change_positive_ = value;
}

// -------------------------------------------------------------------

// QuotationMessage

// required string code = 1;
inline bool QuotationMessage::has_code() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void QuotationMessage::set_has_code() {
  _has_bits_[0] |= 0x00000001u;
}
inline void QuotationMessage::clear_has_code() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void QuotationMessage::clear_code() {
  if (code_ != &::google::protobuf::internal::kEmptyString) {
    code_->clear();
  }
  clear_has_code();
}
inline const ::std::string& QuotationMessage::code() const {
  return *code_;
}
inline void QuotationMessage::set_code(const ::std::string& value) {
  set_has_code();
  if (code_ == &::google::protobuf::internal::kEmptyString) {
    code_ = new ::std::string;
  }
  code_->assign(value);
}
inline void QuotationMessage::set_code(const char* value) {
  set_has_code();
  if (code_ == &::google::protobuf::internal::kEmptyString) {
    code_ = new ::std::string;
  }
  code_->assign(value);
}
inline void QuotationMessage::set_code(const char* value, size_t size) {
  set_has_code();
  if (code_ == &::google::protobuf::internal::kEmptyString) {
    code_ = new ::std::string;
  }
  code_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* QuotationMessage::mutable_code() {
  set_has_code();
  if (code_ == &::google::protobuf::internal::kEmptyString) {
    code_ = new ::std::string;
  }
  return code_;
}
inline ::std::string* QuotationMessage::release_code() {
  clear_has_code();
  if (code_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = code_;
    code_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void QuotationMessage::set_allocated_code(::std::string* code) {
  if (code_ != &::google::protobuf::internal::kEmptyString) {
    delete code_;
  }
  if (code) {
    set_has_code();
    code_ = code;
  } else {
    clear_has_code();
    code_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// repeated .QuotationMessage.QuotationFillMessage quote_fill_msg = 2;
inline int QuotationMessage::quote_fill_msg_size() const {
  return quote_fill_msg_.size();
}
inline void QuotationMessage::clear_quote_fill_msg() {
  quote_fill_msg_.Clear();
}
inline const ::QuotationMessage_QuotationFillMessage& QuotationMessage::quote_fill_msg(int index) const {
  return quote_fill_msg_.Get(index);
}
inline ::QuotationMessage_QuotationFillMessage* QuotationMessage::mutable_quote_fill_msg(int index) {
  return quote_fill_msg_.Mutable(index);
}
inline ::QuotationMessage_QuotationFillMessage* QuotationMessage::add_quote_fill_msg() {
  return quote_fill_msg_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::QuotationMessage_QuotationFillMessage >&
QuotationMessage::quote_fill_msg() const {
  return quote_fill_msg_;
}
inline ::google::protobuf::RepeatedPtrField< ::QuotationMessage_QuotationFillMessage >*
QuotationMessage::mutable_quote_fill_msg() {
  return &quote_fill_msg_;
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_WINNERLib_2fquotation_5fmsg_2eproto__INCLUDED
