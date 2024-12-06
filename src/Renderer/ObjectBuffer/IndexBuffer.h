#pragma once
class IndexBuffer
{
private:
	unsigned int m_Id = 0;
	unsigned int m_Count = 0;
public:
	IndexBuffer() = default;
	//triangle count = 3
	//rectangle count = 4 , size = 6 (num of indices)
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();
	

	void Bind() const;
	void Unbind() const;
	void DeleteBuffer() const;

	inline unsigned int GetCount() const { return m_Count; }
};