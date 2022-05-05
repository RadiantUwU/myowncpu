void memcopy(void* src, void* dest, unsigned long long size) {
    char* src_ = (char*)src;
    char* dest_ = (char*)dest;
    for (unsigned long long i = 0; i < size; i++) {
        dest_[i] = src_[i];
    }
}
unsigned long long roundinchunks(unsigned long long num, unsigned long long chunksize) {
    if ((num % chunksize) == 0) return num;
    return (chunksize - (num % chunksize) + num);
}
inline long long positive_modulo(long long i, unsigned long long n) {
    return (i % n + n) % n;
}
template <typename T>
class ListIterator;
template <typename T>
class ListIterator {
public:
    ListIterator(void* pos) {
        this->pos = pos;
        this->size = sizeof(T);
    }
    ListIterator(void* pos, size_t size) {
        this->pos = pos;
        this->size = size;
    }
    T& operator* () {
        return *((T*)this->pos);
    }
    ListIterator operator+ (const long long other) {
        return ListIterator<T>(this->pos + (other * this->size),this->size);
    }
    ListIterator operator- (const long long other) {
        return ListIterator<T>(this->pos - (other * this->size),this->size);
    }
    ListIterator& operator ++() {
        return (*this) + 1;
    }
    ListIterator& operator --() {
        return (*this) - 1;
    }
    bool operator ==(const ListIterator& other) {
        return this->pos == other.pos && this->size == other.size;
    }
    bool operator !=(const ListIterator& other) {
        return not this->operator==(other);
    }
private:
    void* pos;
    size_t size;
};
template <typename T>
class List {
public:
    ListIterator<T> begin() {
        return ListIterator<T>(this->beginning);
    };
    ListIterator<T> end() {
        return ListIterator<T>(this->beginning) + this->length;
    };
    ListIterator<T> rbegin() {
        return ListIterator<T>(this->beginning + ((long long)sizeof(T) * (this->length - 1)), (long long)sizeof(T) * -1);
    }
    ListIterator<T> rend() {
        return ListIterator<T>(this->beginning + ((long long)sizeof(T) * -1), (long long)sizeof(T) * -1);
    }
    ~List() {
        if (this->beginning != nullptr) free(this->beginning);
    }
    bool resize(unsigned long long len) {
        if (len == this->length) return true;
        if (len > this->allocatedsize && this->beginning != nullptr) {
            unsigned long long newallocsize = roundinchunks(len,8);
            void* newp = std::realloc(this->beginning,newallocsize * sizeof(T));
            if (newp == nullptr) return false;
            free(this->beginning);
            this->beginning = newp;
            this->allocated = newallocsize;
            this->length = len;
            return true;
        }
        else if (this->beginning == nullptr) {
            unsigned long long newallocsize = roundinchunks(len,8);
            void* newp = malloc(sizeof(T) * newallocsize);
            if (newp == nullptr) return false;
            this->beginning = newp;
            this->allocated = newallocsize;
            this->length = len;
            return true;
        }
        else {
            this->length = len;
            return true;
        }
    }
    unsigned long long size() {return this->length;};
    unsigned long long maxsize() {return this->allocatedsize;};
    T& pop_back() {
        if (this->length != 0) {
            T& t = *(this->end() - 1);
            this->length--;
            return t;
        }
        return NULL;
    }
    bool push_back(T& obj) {
        if (not this->resize(this->length + 1)) return false;
        (*this)[-1] = obj;
        return true;
    }
    T& front() {
        return (*this)[0];
    }
    T& back() {
        return (*this)[-1];
    }
    void clear() {
        if (this->beginning == nullptr) return;
        free(this->beginning);
        this->length = 0;
        this->allocatedsize = 0;
    }
    void erase(long long in) {
        if (this->length == 0) return;
        in = positive_modulo(in,this->length);
        for (auto it = this->begin() + in; (it + 1) != this->end(); it++) {
            *(it) = *(it + 1); 
        }
        this->length--;
    }
    T& operator [] (long long in) {
        if (this->length == 0) return NULL;
        return *(ListIterator<T>(this->beginning) + positive_modulo(in,this->length));
    }// chihiro owo
private:
    void* beginning = nullptr;
    unsigned long long length;
    unsigned long long allocatedsize;
};