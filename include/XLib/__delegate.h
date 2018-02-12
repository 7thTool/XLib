//#ifndef _H__DELEGATE_H_
//#define _H__DELEGATE_H_

#include <list>
using namespace std;

template <typename ReturnType TEMPLATE_LIST>
class ICallback<ReturnType(TYPE_LIST)>
{
    typedef ICallback<ReturnType(TYPE_LIST)> This;

public:
    virtual ~ICallback() {}

    virtual ReturnType invoke(TYPE_LIST) const = 0;
    virtual bool equals(const This* pDelegate) const = 0;
    virtual This* clone() const = 0;
};

template <typename ReturnType TEMPLATE_LIST>
class NativeCallback<ReturnType(TYPE_LIST)>
    : public ICallback<ReturnType(TYPE_LIST)>
{
    typedef ICallback<ReturnType(TYPE_LIST)> Base;
    typedef NativeCallback<ReturnType(TYPE_LIST)> This;
    typedef ReturnType (*Callback)(TYPE_LIST);
public:
    explicit NativeCallback(Callback ptr)
        : m_ptr(ptr)
    {
    }

    NativeCallback(const This& rhs)
        : ICallback<ReturnType(TYPE_LIST)>(rhs)
        , m_ptr(rhs.m_ptr)
    {
    }

//private:
	~NativeCallback()
	{
	}

public:
    ReturnType invoke(TYPE_PARAM_LIST) const
    {
        return (*m_ptr)(PARAM_LIST);
    }

    bool equals(const Base* pObj) const
    {
        const This* pRhs = dynamic_cast<const This*>(pObj);
		if (pRhs == 0) {
			return false;
		}
		if (m_ptr == pRhs->m_ptr) {
			return true;
		}
        return false;
    }

    This* clone() const
    {
        return new This(*this);
    }

private:
    Callback m_ptr;
};

template <typename ObjectType, typename ReturnType TEMPLATE_LIST>
class ObjectCallback<ObjectType, ReturnType(TYPE_LIST)>
    : public ICallback<ReturnType(TYPE_LIST)>
{
    typedef ICallback<ReturnType(TYPE_LIST)> Base;
    typedef ObjectCallback<ObjectType, ReturnType(TYPE_LIST)> This;
    typedef ReturnType (ObjectType::*Callback)(TYPE_LIST);
    typedef ReturnType (ObjectType::*ConstCallback)(TYPE_LIST) const;

    enum 
	{ 
		CONST_OBJECT	= 0X01,
		CONST_CALLBACK	= 0X02, 
		NEED_DELETE		= 0X04, 
		DONT_DELETE		= 0X08,
	};
   
public:
    ObjectCallback(ObjectType* t, Callback ptr, bool isNeedDelete = false)
    {
        m_obj = t;
		m_callback = ptr;
        m_flag = isNeedDelete ? NEED_DELETE : DONT_DELETE;
        m_refcount = 0;
        incrementrefcount();
    }

    ObjectCallback(ObjectType* t, ConstCallback ptr, bool isNeedDelete = false)
    {
		m_obj = t;
		m_callback = ptr;
		m_flag = CONST_CALLBACK | (isNeedDelete ? NEED_DELETE : DONT_DELETE);
		m_refcount = 0;
        incrementrefcount();
    }

    ObjectCallback(const ObjectType* t, ConstCallback ptr)
    {
		m_obj = t;
		m_callback = ptr;
		m_flag = CONST_OBJECT|CONST_CALLBACK;
		m_refcount = 0;
        incrementrefcount();
    }

    ObjectCallback(const This& rhs)
        : ICallback<ReturnType(TYPE_LIST)>(rhs)
		, m_flag(rhs.m_flag)
		, m_refcount(rhs.m_refcount)
        , m_obj(rhs.m_obj)
		, m_callback(rhs.m_callback)
    {
        incrementrefcount();
    }

//private:
    ~ObjectCallback()
    {
        decrementrefcount();
    }

public:
    ObjectCallback& operator=(const This& rhs)
    {
        if (this == &rhs)
            return *this;
        decrementrefcount();
        m_flag = rhs.m_flag;
		m_refcount = rhs.m_refcount;
		m_obj = rhs.m_obj;
		m_callback = rhs.m_callback;
        incrementrefcount();
        return *this;
    }

    ReturnType invoke(TYPE_PARAM_LIST) const
    {
		if(m_flag&CONST_CALLBACK) {
			if (m_flag == CONST_OBJECT) {
				return (m_constobj->*(m_constcallback))(PARAM_LIST);
			} else {
				return (m_obj->*(m_callback))(PARAM_LIST);
			}
		} else {
			if (m_flag == CONST_OBJECT) {
				return (m_constobj->*(m_constcallback))(PARAM_LIST);
			} else {
				return (m_obj->*(m_callback))(PARAM_LIST);
			}
		}
    }

    bool equals(const Base* pObj) const
    {
        const This* pRhs = dynamic_cast<const This*>(pObj);
		if (pRhs == 0) {
			return false;
		}
        if (m_obj==pRhs->m_obj
			&& m_callback == pRhs->m_callback
		//	&& m_flag == m_flag
		) {
            return true;
		}
        return false;
    }

    This* clone() const
    {
        return new This(*this);
    }

private:
    void incrementrefcount()
    {
        ++m_refcount;
    }

    void decrementrefcount()
    {
        if (--m_refcount == 0)
        {
			if (m_flag&NEED_DELETE) {
                delete m_obj;
			}
        }
    }

private:
	bool m_flag;
	int m_refcount;
	union 
	{ 
		ObjectType* m_obj; 
		const ObjectType* m_constobj;
	};
	union 
	{ 
		Callback m_callback;
		ConstCallback m_constcallback;
	};
};

template <typename ReturnType TEMPLATE_LIST, typename LockType>
class delegate<ReturnType(TYPE_LIST), LockType>
    : public ICallback<ReturnType(TYPE_LIST)>
    , public ThreadingModel<LockType>
{
    typedef ICallback<ReturnType(TYPE_LIST)> Base;
    typedef delegate<ReturnType(TYPE_LIST), LockType> This;
    typedef ThreadingModel<LockType> ThreadingModelType;

	typedef std::list<Base*> callbacklist;
public:
    delegate() {}

    explicit delegate(ReturnType (*ptr)(TYPE_LIST))
    {
        this->add(ptr);
    }

    template <typename ObjectType>
    delegate(const ObjectType* t, ReturnType(ObjectType::*ptr)(TYPE_LIST) const)
    {
        this->add(t, ptr);
    }

    template <typename ObjectType>
    delegate(ObjectType* t, ReturnType(ObjectType::*ptr)(TYPE_LIST),
            bool isNeedDelete = false)
    {
        this->add(t, ptr, isNeedDelete);
    }

    template <typename ObjectType>
    delegate(ObjectType* t, ReturnType(ObjectType::*ptr)(TYPE_LIST) const,
            bool isNeedDelete = false)
    {
        this->add(t, ptr, isNeedDelete);
    }

    template <typename FunctorType>
    explicit delegate(const FunctorType* pFunctor)
    {
        this->add(pFunctor);
    }

    template <typename FunctorType>
    explicit delegate(FunctorType* pFunctor, bool isNeedDelete = false)
    {
        this->add(pFunctor, isNeedDelete);
    }

    delegate(const This& rhs)
        : Base(rhs)
        , ThreadingModelType()
    {
        copyFrom(rhs);
    }

    This& operator=(const This& rhs)
    {
		if (this == &rhs) {
            return *this;
		}
        this->release();
        copyFrom(rhs);
        return *this;
    }

    ~delegate()
    {
        release();
    }

    void release()
    {
        typename ThreadingModelType::Lock guard(*this);
		callbacklist::iterator it = m_ptrs.begin();
        while (it!=m_ptrs.end())
        {
			Base* ptr = (*it);
			if (ptr) {
				delete ptr;
				ptr = NULL;
			}
			++it;
        }
        m_ptrs.clear();
    }

    ReturnType operator()(TYPE_PARAM_LIST)
    {
        return this->invoke(PARAM_LIST);
    }

    ReturnType invoke(TYPE_PARAM_LIST) const
    {
        typename ThreadingModelType::Lock guard(*this);
		if (m_ptrs.size() == 1) {
			const Base* const & ptr = m_ptrs.front();
			if(ptr) {
				return ptr->invoke(PARAM_LIST);
			}
		} else {
			callbacklist::const_iterator it = m_ptrs.begin();
			while (it!=m_ptrs.end())
			{
				Base* ptr = (*it);
				if(ptr) {
					ptr->invoke(PARAM_LIST);
				}
				++it;
			}
		}
		return ReturnType();
    }

    bool equals(const Base* pObj) const
    {
        const This* pRhs = dynamic_cast<const This*>(pObj);
		if (pRhs == 0) {
			return false;
		}
		if (pRhs==this) {
			return true;
		}

        This* ptrForDelete = 0;
        const This* pClone;
        cloneForComparison(pRhs, pClone, ptrForDelete, typename ThreadingModelType::ThreadTag());

        typename ThreadingModelType::Lock guard(*this);
        callbacklist::const_iterator it1 = m_ptrs.begin();
        callbacklist::const_iterator it2 = pClone->m_ptrs.begin();
        while (it1 != m_ptrs.end() && it2 != pClone->m_ptrs.end())
        {
			Base* ptr1 = *it1;
			Base* ptr2 = *it2;
            if (!ptr1->equals(ptr2))
                break;
            ++it1;
			++it2;
        }

		if (ptrForDelete) {
			delete ptrForDelete;
		}
        
        return (it1 == m_ptrs.end() && it2 == pClone->m_ptrs.end());
    }

    This* clone() const
    {
        This* pClone = new This();
        typename ThreadingModelType::Lock guard(*this);
        callbacklist::const_iterator it = m_ptrs.begin();
        while (it!=m_ptrs.end())
        {
			Base* ptr = (*it);
			if (ptr) {
				Base* newptr = ptr->clone();
				pClone->m_ptrs.push_back(newptr);
			}
			++it;
        }
        return pClone;
    }

    void add(ReturnType (*ptr)(TYPE_LIST))
    {
        Base* pNew = new NativeCallback<ReturnType(TYPE_LIST)>(ptr);
        this->addImpl(pNew);
    }

    template <typename ObjectType>
    void add(const ObjectType* t, ReturnType(ObjectType::*ptr)(TYPE_LIST) const)
    {
        Base* pNew = new ObjectCallback<ObjectType, ReturnType(TYPE_LIST)>(t, ptr);
        this->addImpl(pNew);
    }

    template <typename ObjectType>
    void add(ObjectType* t, ReturnType(ObjectType::*ptr)(TYPE_LIST), bool isNeedDelete = false)
    {
        Base* pNew = new ObjectCallback<ObjectType, ReturnType(TYPE_LIST)>
                (t, ptr, isNeedDelete);
        this->addImpl(pNew);
    }

    template <typename ObjectType>
    void add(ObjectType* t, ReturnType(ObjectType::*ptr)(TYPE_LIST) const,
            bool isNeedDelete = false)
    {
        Base* pNew = new ObjectCallback<ObjectType, ReturnType(TYPE_LIST)>
            (t, ptr, isNeedDelete);
        this->addImpl(pNew);
    }

    template <typename FunctorType>
    void add(FunctorType* pFunctor, bool isNeedDelete = false)
    {
        this->add(pFunctor, &FunctorType::operator(), isNeedDelete);
    }

    template <typename FunctorType>
    void add(const FunctorType* pFunctor)
    {
        this->add(pFunctor, &FunctorType::operator());
    }

    void add(const This& rhs)
    {
        This* pClone = rhs.clone();
        this->addImpl(pClone);
    }

    void remove(ReturnType (*ptr)(TYPE_LIST))
    {
        Base* pNew = new NativeCallback<ReturnType(TYPE_LIST)>(ptr);
        this->removeImpl(pNew);
    }

    template <typename ObjectType>
    void remove(const ObjectType* t, ReturnType(ObjectType::*ptr)(TYPE_LIST) const)
    {
        Base* pNew = new ObjectCallback<ObjectType, ReturnType(TYPE_LIST)>(t, ptr);
        this->removeImpl(pNew);
    }

    template <typename ObjectType>
    void remove(ObjectType* t, ReturnType(ObjectType::*ptr)(TYPE_LIST), bool isNeedDelete = false)
    {
        Base* pNew = new ObjectCallback<ObjectType, ReturnType(TYPE_LIST)>
            (t, ptr, isNeedDelete);
        this->removeImpl(pNew);
    }

    template <typename ObjectType>
    void remove(ObjectType* t, ReturnType(ObjectType::*ptr)(TYPE_LIST) const,
            bool isNeedDelete = false)
    {
        Base* pNew = new ObjectCallback<ObjectType, ReturnType(TYPE_LIST)>
            (t, ptr, isNeedDelete);
        this->removeImpl(pNew);
    }

    template <typename FunctorType>
    void remove(FunctorType* pFunctor, bool isNeedDelete = false)
    {
        this->remove(pFunctor, &FunctorType::operator(), isNeedDelete);
    }

    template <typename FunctorType>
    void remove(const FunctorType* pFunctor)
    {
        this->remove(pFunctor, &FunctorType::operator());
    }

    void remove(const This& rhs)
    {
        this->remove(rhs, typename ThreadingModelType::ThreadTag());
    }

	//下面只是一个例子，后面可以补充成add和remove那样丰富

	This& operator + (ReturnType (*ptr)(TYPE_LIST))
	{
		add(ptr);
		return *this;
	}

	This& operator - (ReturnType (*ptr)(TYPE_LIST))
	{
		remove(ptr);
		return This;
	}

	void operator += (ReturnType (*ptr)(TYPE_LIST))
	{
		add(ptr);
	}

	void operator -= (ReturnType (*ptr)(TYPE_LIST))
	{
		remove(ptr);
	}

private:
    void cloneForComparison(const This* pRhs, const This*& pClone,
            This*& ptrForDelete, SingleThreadTag) const
    {
        pClone = pRhs;
        ptrForDelete = 0;
    }

    void cloneForComparison(const This* pRhs, const This*& pClone,
            This*& ptrForDelete, MultiThreadTag) const
    {
            ptrForDelete = pRhs->clone();
            pClone = ptrForDelete;
    }

    void copyFrom(const This& rhs)
    {
        This* pClone = rhs.clone();
        m_ptrs = pClone->m_ptrs;
        delete pClone;
    }

    void remove(const This& rhs, SingleThreadTag)
    {
        this->removeImpl(&rhs);
    }

    void remove(const This& rhs, MultiThreadTag)
    {
        this->removeImpl(rhs.clone());
    }

    void addImpl(Base* pRhs)
    {
        typename ThreadingModelType::Lock guard(*this);
		m_ptrs.push_back(pRhs);
    }

    void removeImpl(const Base* pRhs)
    {
        typename ThreadingModelType::Lock guard(*this);
		callbacklist::iterator it = m_ptrs.begin();
        while (it!=m_ptrs.end())
        {
			Base* ptr = *it;
            if (ptr->equals(pRhs)) {
				it = m_ptrs.erase(it);
                delete pRhs;
                break;
            }
			++it;
        }
    }

    void removeImpl(Base* pRhs)
    {
        this->removeImpl((const Base*)pRhs);
    }

private:
	callbacklist m_ptrs;
};

//#endif//_H__DELEGATE_H_
