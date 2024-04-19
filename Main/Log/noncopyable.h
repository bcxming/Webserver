#pragma once
class noncopyable {
	//禁止拷贝和赋值
protected:
	noncopyable(){}
	~noncopyable(){}
private:
	noncopyable(const noncopyable&);
	const noncopyable& operator = (const noncopyable&);
};