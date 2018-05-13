#ifndef HELPER_H
#define HELPER_H
template <typename Ptr> class PointerContainer
{
    Ptr * ptr;
public:
    PointerContainer(Ptr * p = nullptr) : ptr(p) {}
    PointerContainer(const PointerContainer<Ptr>& pc) : ptr(pc.getPtr()) {}
    PointerContainer<Ptr>& operator=(const PointerContainer<Ptr>& cpc) { ptr = cpc.getPtr(); }

    Ptr * getPtr() const { return ptr; }
};
#endif // HELPER_H
