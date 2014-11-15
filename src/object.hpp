#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>
#include "value.hpp"


namespace nscheme {

class Inst;
class LabelInst;
struct Context;


class Object {
public:
    virtual ~Object() {}
    virtual std::string toString() const = 0;
    virtual void mark() = 0;
    virtual size_t size() const = 0;

    bool isMarked() const { return marked_; }

    void resetMark() { marked_ = false; }

protected:
    bool marked_ = false;
};


class StringObject : public Object {
public:
    StringObject(const std::string& str)
        : str_(str)
    {
    }

    std::string toString() const override;

    void mark() override { marked_ = true; }

    size_t size() const override { return sizeof(*this); }

private:
    std::string str_;
};


class RealObject : public Object {
public:
    RealObject(double real)
        : real_(real)
    {
    }

    std::string toString() const override { return std::to_string(real_); }

    void mark() override { marked_ = true; }

    size_t size() const override { return sizeof(*this); }

private:
    double real_;
};


class PairObject : public Object {
public:
    PairObject(Value car, Value cdr)
        : car_(car)
        , cdr_(cdr)
    {
    }

    Value getCar() { return car_; }

    const Value getCar() const { return car_; }

    void setCar(Value car) { car_ = car; }

    Value getCdr() { return cdr_; }

    const Value getCdr() const { return cdr_; }

    void setCdr(Value cdr) { cdr_ = cdr; }

    std::string toString() const override;

    void mark() override;

    size_t size() const override { return sizeof(*this); }

private:
    Value car_;
    Value cdr_;
};


class VectorObject : public Object {
public:
    VectorObject() {}

    VectorObject(size_t length, Value fill)
        : values_(length, fill)
    {
    }

    size_t getLength() const noexcept { return values_.size(); }

    Value get(size_t index) { return values_[index]; }

    const Value get(size_t index) const { return values_[index]; }

    void add(Value value) { values_.push_back(value); }

    void set(size_t index, Value value) { values_[index] = value; }

    std::string toString() const override;

    void mark() override;

    size_t size() const override { return sizeof(*this); }

private:
    std::vector<Value> values_;
};


class Frame : public Object {
public:
    Frame(Frame* parent, const std::vector<Value>& variables)
        : parent_(parent)
        , variables_(variables)
    {
    }

    const Frame* getParent() const { return parent_; }

    Frame* getParent() { return parent_; }

    const std::vector<Value>& getVariables() const { return variables_; }

    std::vector<Value>& getVariables() { return variables_; }

    std::string toString() const { return "<frame>"; }

    void mark() override;

    size_t size() const override { return sizeof(*this); }

private:
    Frame* parent_;
    std::vector<Value> variables_;
};


class ClosureObject : public Object {
public:
    ClosureObject(LabelInst* label, Frame* frame, size_t arg_size, size_t frame_size)
        : label_(label)
        , frame_(frame)
        , arg_size_(arg_size)
        , frame_size_(frame_size)
    {
    }

    LabelInst* getLabel() const noexcept { return label_; }

    Frame* getFrame() const noexcept { return frame_; }

    size_t getArgSize() const noexcept { return arg_size_; }

    size_t getFrameSize() const noexcept { return frame_size_; }

    std::string toString() const override
    {
        return "<closure " + std::to_string((uintptr_t)label_) + ">";
    }

    void mark() override;

    size_t size() const override { return sizeof(*this); }

private:
    LabelInst* label_;
    Frame* frame_;
    size_t arg_size_;
    size_t frame_size_;
};


class CFunctionObject : public Object {
public:
    CFunctionObject(const std::function<void(Context*, size_t)>& func, const std::string& name)
        : func_(func)
        , name_(name)
    {
    }

    void call(Context* ctx, size_t n_args) { func_(ctx, n_args); }

    std::string toString() const override { return "<c_function " + name_ + ">"; }

    void mark() override;

    size_t size() const override { return sizeof(*this); }

private:
    std::function<void(Context*, size_t)> func_;
    std::string name_;
};


class ContinuationObject : public Object {
public:
    ContinuationObject(Inst** ip, const std::vector<Value>& value_stack,
                       const std::vector<Inst**>& control_stack,
                       const std::vector<Frame*>& frame_stack)
        : ip_(ip)
        , value_stack_(value_stack)
        , control_stack_(control_stack)
        , frame_stack_(frame_stack)
    {
    }

    Inst** getInstrunctionPointer() { return ip_; }

    std::vector<Value>& getValueStack() { return value_stack_; }

    std::vector<Inst**>& getControlStack() { return control_stack_; }

    std::vector<Frame*>& getFrameStack() { return frame_stack_; }

    std::string toString() const override { return "<continuation>"; }

    void mark() override;

    size_t size() const override { return sizeof(*this); }

private:
    Inst** ip_;
    std::vector<Value> value_stack_;
    std::vector<Inst**> control_stack_;
    std::vector<Frame*> frame_stack_;
};


// utility functions
bool isPair(Value v);


} // namespace nscheme
