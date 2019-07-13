
class border_queue {
	int size;
	int head;
	int tail;
	int *buffer_x;
	int *buffer_y;

public:
	border_queue(const int size);
	~border_queue();
	void copy(border_queue *bq_src);

	void border_enqueue(int x, int y);
	bool border_dequeue(int *x, int *y); // return false if queue is empty, true otherwise
	void print();
};
