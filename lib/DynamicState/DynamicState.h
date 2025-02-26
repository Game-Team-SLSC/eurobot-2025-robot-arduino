#pragma once

class DynamicStateBase {
public:
    virtual ~DynamicStateBase() {}
    virtual void reset() = 0;
    virtual bool hasChanged() const = 0;
};

template <typename T>
class DynamicState : public DynamicStateBase {
public:
    DynamicState(T initialValue);

    T get() const;
    void set(T value);

    bool hasChanged() const override;
    void reset() override;

private:
    T value;
    bool changed;
};

template <typename T>
DynamicState<T>::DynamicState(T initialValue) : value(initialValue), changed(true) {}

template <typename T>
T DynamicState<T>::get() const {
    return value;
}

template <typename T>
void DynamicState<T>::set(T v) {
    if (value == v) return;

    value = v;
    changed = true;
}

template <typename T>
bool DynamicState<T>::hasChanged() const {
    return changed;
}

template <typename T>
void DynamicState<T>::reset() {
    changed = false;
}