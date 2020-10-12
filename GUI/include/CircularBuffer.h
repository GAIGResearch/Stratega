#pragma once
template<typename T>
class CircularBuffer
{
    CircularBuffer();

    const int size;
    std::vector<T> data;
    int front;
    int count;
public:
    CircularBuffer(int);
     
    bool empty() { return count == 0; }
    bool full() { return count == size; }
	bool hasElement(int index) const { return front > index; }
	T getElement(int index) const { return data[index]; }
    int getFront() const { return ((front + (count-1)) % size); }
    int getCount() const { return count; }
    int getSize() const { return size; }

	//Get relative position from the front of the circular buffer
	int getRelativePosition(int index) const 
    {
        int pos ;
    	if(index<=getFront())
    	{
            pos = size + (index - getFront());
    		
    	}
        else
        {
            pos = (index - getFront());
        }
        return pos;
    }
	
    bool add(const T&);
    bool remove(T*);
};

template<typename T>
CircularBuffer<T>::CircularBuffer(int sz) : size(sz) {
    if (sz == 0) throw std::invalid_argument("size cannot be zero");
    data.resize(sz);
    front = 0;
    count = 0;
}

template<typename T>
bool CircularBuffer<T>::add(const T& t)
{
      // find index where insert will occur
      int end = (front + count) % size;
      data[end] = t;
      count++;
      return true;    
}

// returns true if there is something to remove, false otherwise
template<typename T>
bool CircularBuffer<T>::remove(T* t) {
    if (empty()) {
        return false;
    }
    else {
        *t = data[front];

        front = front == size ? 0 : front + 1;
        count--;
        return true;
    }
}