class Picker
{
public:
	Picker() = default;
	~Picker() = default;
	int GetPickedItem() const;
private:
	int m_currentItem = -1;
};