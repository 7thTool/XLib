#ifndef _H_MAP_H_
#define _H_MAP_H_

#include "XLib.h"

template<class _Ty,
	class _Pr = less<_Ty>,
	class _Alloc = allocator<pair<const _Ty, _Ty> > >
class multirmap : public multimap<_Ty,_Ty,_Pr,_Alloc>
{
public:
	typedef multirmap<_Ty,_Pr,_Alloc> _Myt;
	typedef multimap<_Ty,_Ty,_Pr,_Alloc> _Mybase;
	typedef _Ty key_type;
	typedef _Ty mapped_type;
	typedef _Ty referent_type;	// retained
	typedef _Pr key_compare;
	typedef typename _Mybase::value_compare value_compare;
	typedef typename _Mybase::allocator_type allocator_type;
	typedef typename _Mybase::size_type size_type;
	typedef typename _Mybase::difference_type difference_type;
	typedef typename _Mybase::pointer pointer;
	typedef typename _Mybase::const_pointer const_pointer;
	typedef typename _Mybase::reference reference;
	typedef typename _Mybase::const_reference const_reference;
	typedef typename _Mybase::iterator iterator;
	typedef typename _Mybase::const_iterator const_iterator;
	typedef typename _Mybase::reverse_iterator reverse_iterator;
	typedef typename _Mybase::const_reverse_iterator 
		const_reverse_iterator;
	typedef typename _Mybase::value_type value_type;

	multirmap()
		: _Mybase(key_compare(), allocator_type())
	{	// construct empty map from defaults
	}

	explicit multirmap(const key_compare& _Pred)
		: _Mybase(_Pred, allocator_type())
	{	// construct empty map from comparator
	}
	multirmap(const key_compare& _Pred, const allocator_type& _Al)
		: _Mybase(_Pred, _Al)
	{	// construct empty map from comparator and allocator
	}

	template<class _Iter>
	multirmap(_Iter _First, _Iter _Last)
		: _Mybase(key_compare(), allocator_type())
	{	// construct map from [_First, _Last), defaults
		for (; _First != _Last; ++_First)
			insert(*_First);
	}

	template<class _Iter>
	multirmap(_Iter _First, _Iter _Last,
		const key_compare& _Pred)
		: _Mybase(_Pred, allocator_type())
	{	// construct map from [_First, _Last), comparator
		for (; _First != _Last; ++_First)
			insert(*_First);
	}

	template<class _Iter>
	multirmap(_Iter _First, _Iter _Last,
		const key_compare& _Pred, const allocator_type& _Al)
		: _Mybase(_Pred, _Al)
	{	// construct map from [_First, _Last), comparator, and allocator
		for (; _First != _Last; ++_First)
			insert(*_First);
	}

	iterator insert(const value_type& _Val)
	{	// insert a {key, mapped} value
		_Mybase::insert(value_type(_Val.second,_Val.first));
		return (_Mybase::insert(_Val));
	}

	iterator insert(const_iterator _Where, const value_type& _Val)
	{	// insert a {key, mapped} value, with hint
		_Mybase::insert(value_type(_Val.second,_Val.first));
		return (_Mybase::insert(_Where, _Val));
	}

	void erase(const_iterator _Where)
	{	// erase element at _Where
		_Mybase::erase(_Where->second);
		_Mybase::erase(_Where);
	}

	size_type erase(const key_type& _Keyval)
	{	// erase and count all that match _Keyval
		_Paircc pr = _Mybase::equal_range(_Keyval);
		size_type _Num = 0;
		_Distance(pr.first, pr.second, _Num);
		for (const_iterator cit = pr.first; cit!=pr.second;++cit)
			_Mybase::erase(cit->second);
		
		(_Mybase::erase(pr.first,pr.second));

		return (_Num);
	}

	void erase(const_iterator _First, const_iterator _Last)
	{	// erase [_First, _Last)
		for (const_iterator cit=_First; cit != _Last; ++cit)
			_Mybase::erase(cit->second);

		_Mybase::erase(_First, _Last);
	}

	template<class _Iter>
	void insert(_Iter _First, _Iter _Last)
	{	// insert [_First, _Last), arbitrary iterators
		for (; _First != _Last; ++_First)
			insert(*_First);
	}

	void rerase(const_iterator _Where)
	{	// erase element at _Where
		_Paircc pr = _Mybase::equal_range(_Where->second);
		for (const_iterator cit = pr.first; cit!=pr.second;++cit)
			if (!_DEBUG_LT_PRED(this->comp, _Where->first, cit->second) 
				&& !_DEBUG_LT_PRED(this->comp, cit->second, _Where->first)) {
			//if(_Where->first==cit->second) {
				_Mybase::erase(cit);
				break;
			}

		_Mybase::erase(_Where);
	}

	void rerase(const key_type& _Keyval)
	{	// erase and count all that match _Keyval
		erase(_Mybase::find(_Keyval));
	}

	void rerase(const_iterator _First, const_iterator _Last)
	{	// erase [_First, _Last)
		for (; _First != _Last; ++_First)
			rerase(_First);
	}

protected:
};

template<class _Kty,
class _Ty,
class _Kpr = less<_Kty>,
class _Pr = less<_Ty>,
class _Kalloc = allocator<pair<const _Kty, _Ty> >,
class _Alloc = allocator<pair<const _Ty, _Kty> > >
class multixmap 
	: public multimap<_Kty, _Ty, _Kpr, _Kalloc>
	, public map<_Ty,_Kty, _Pr, _Alloc>
{
public:
	typedef multixmap<_Kty,_Ty,_Kpr,_Pr,_Kalloc,_Alloc> _Myt;
	typedef multimap<_Kty, _Ty, _Kpr, _Kalloc> _Myleft;
	typedef map<_Ty,_Kty, _Pr, _Alloc> _Myright;
	typedef _Kty left_type;
	typedef _Ty right_type;
	typedef _Kpr left_compare;
	typedef _Pr right_compare;

	typedef _Kty key_type;
	typedef _Ty mapped_type;
	typedef _Ty referent_type;	// retained
	typedef _Kpr key_compare;

	typedef typename _Myleft::value_compare value_compare;
	typedef typename _Myleft::allocator_type allocator_type;
	typedef typename _Myleft::size_type size_type;
	typedef typename _Myleft::difference_type difference_type;
	typedef typename _Myleft::pointer pointer;
	typedef typename _Myleft::const_pointer const_pointer;
	typedef typename _Myleft::reference reference;
	typedef typename _Myleft::const_reference const_reference;
	typedef typename _Myleft::iterator iterator;
	typedef typename _Myleft::const_iterator const_iterator;
	typedef typename _Myleft::reverse_iterator reverse_iterator;
	typedef typename _Myleft::const_reverse_iterator
		const_reverse_iterator;
	typedef typename _Myleft::_Pairib _Pairib;
	typedef typename _Myleft::_Pairii _Pairii;
	typedef typename _Myleft::_Paircc _Paircc;
	typedef typename _Myleft::value_type value_type;

	typedef _Ty key_right_type;
	typedef _Kty mapped_right_type;
	typedef _Kty referent_right_type;	// retained
	typedef _Pr key_right_compare;

	typedef typename _Myright::value_compare value_right_compare;
	typedef typename _Myright::allocator_type allocator_right_type;
	typedef typename _Myright::size_type size_right_type;
	typedef typename _Myright::difference_type difference_right_type;
	typedef typename _Myright::pointer right_pointer;
	typedef typename _Myright::const_pointer const_right_pointer;
	typedef typename _Myright::reference right_reference;
	typedef typename _Myright::const_reference const_right_reference;
	typedef typename _Myright::iterator right_iterator;
	typedef typename _Myright::const_iterator const_right_iterator;
	typedef typename _Myright::reverse_iterator reverse_right_iterator;
	typedef typename _Myright::const_reverse_iterator
		const_reverse_right_iterator;
	typedef typename _Myright::_Pairib _right_Pairib;
	typedef typename _Myright::_Pairii _right_Pairii;
	typedef typename _Myright::_Paircc _right_Paircc;
	typedef typename _Myright::value_type value_right_type;

	multixmap() : _Myleft(),_Myright()
	{	// construct empty map from defaults
	}

	explicit multixmap(const left_compare& _Lpred,const right_compare& _Rpred)
		: _Myleft(_Lpred),_Myright(_Rpred)
	{	// construct empty map from comparator
	}

	multixmap(const left_compare& _Lpred, const allocator_type& _Lal,const right_compare& _Rpred, const allocator_type& _Ral)
		: _Myleft(_Lpred, _Lal),_Myright(_Rpred, _Ral)
	{	// construct empty map from comparator and allocator
	}

	template<class _Iter>
	multixmap(_Iter _First, _Iter _Last)
		: _Myleft(),_Myright()
	{	// construct map from [_First, _Last), defaults
		for (; _First != _Last; ++_First)
			insert(*_First);
	}

	template<class _Iter>
	multixmap(_Iter _First, _Iter _Last,
		const left_compare& _Lpred,const right_compare& _Rpred)
		: _Myleft(_Lpred),_Myright(_Rpred)
	{	// construct map from [_First, _Last), comparator
		for (; _First != _Last; ++_First)
			insert(*_First);
	}

	template<class _Iter>
	multixmap(_Iter _First, _Iter _Last,
		const left_compare& _Lpred, const allocator_type& _Lal,const right_compare& _Rpred, const allocator_type& _Ral)
		: _Myleft(_Lpred, _Lal),right(_Rpred, _Ral)
	{	// construct map from [_First, _Last), comparator, and allocator
		for (; _First != _Last; ++_First)
			insert(*_First);
	}

	//////////////////////////////////////////////////////////////////////////

	iterator insert(const value_type& _Val)
	{	// insert a {key, mapped} value
		_Myright::insert(value_type(_Val.second,_Val.first));
		return (_Myleft::insert(_Val));
	}

	iterator insert(const_iterator _Where, const value_type& _Val)
	{	// insert a {key, mapped} value, with hint
		_Myright::insert(value_type(_Val.second,_Val.first));
		return (_Myleft::insert(_Where, _Val));
	}

	void erase(const_iterator _Where)
	{	// erase element at _Where
		_Myright::erase(_Where->second);
		_Myleft::erase(_Where);
	}

	void erase(const key_type& _Keyval)
	{	// erase and count all that match _Keyval
		_Paircc pr = _Myleft::equal_range(_Keyval);
		for (const_iterator cit = pr.first; cit!=pr.second;++cit)
			_Myright::erase(cit->second);

		(_Myleft::erase(pr.first,pr.second));
	}

	void erase(const_iterator _First, const_iterator _Last)
	{	// erase [_First, _Last)
		for (const_iterator cit=_First; cit != _Last; ++cit)
			_Myright::erase(cit->second);

		_Myleft::erase(_First, _Last);
	}

	template<class _Iter>
	void insert(_Iter _First, _Iter _Last)
	{	// insert [_First, _Last), arbitrary iterators
		for (; _First != _Last; ++_First)
			insert(*_First);
	}

	void rerase(const_right_iterator _Where)
	{	// erase element at _Where
		_Paircc pr = _Myleft::equal_range(_Where->second);
		for (const_iterator cit = pr.first; cit!=pr.second;++cit)
			if (!_DEBUG_LT_PRED(_Myleft::comp, _Where->first, cit->second) 
				&& !_DEBUG_LT_PRED(_Myleft::comp, cit->second, _Where->first)) {
					//if(_Where->first==cit->second) {
					_Myleft::erase(cit);
					break;
			}

			_Myright::erase(_Where);
	}

	void rerase(const right_type& _Keyval)
	{	// erase and count all that match _Keyval
		rerase(_Myright::find(_Keyval));
	}

	void rerase(const_right_iterator _First, const_right_iterator _Last)
	{	// erase [_First, _Last)
		for (; _First != _Last; ++_First)
			rerase(_First);
	}

	_Myleft& left()
	{
		return dynamic_cast<_Myleft&>(*this);
	}

	_Myright& right()
	{
		return dynamic_cast<_Myright&>(*this);
	}

public:
};

template<class _Kty,
	class _Ty,
	class _Kpr = less<_Kty>,
	class _Pr = less<_Ty>,
	class _Kalloc = allocator<pair<const _Kty, _Ty> >,
	class _Alloc = allocator<pair<const _Ty, _Kty> > >
class multi2rmap
{
public:
	typedef multi2rmap<_Kty, _Ty,_Kpr,_Pr,_Kalloc,_Alloc> _Myt;
	typedef multimap<_Kty, _Ty, _Kpr, _Kalloc> _Myleft;
	typedef map<_Ty, _Kty, _Pr, _Alloc> _Myright;
	typedef _Kty left_type;
	typedef _Ty right_type;
	typedef _Kpr left_compare;
	typedef _Pr right_compare;

	typedef typename _Myleft::value_compare value_left_compare;
	typedef typename _Myleft::allocator_type allocator_left_type;
	typedef typename _Myleft::size_type size_left_type;
	typedef typename _Myleft::difference_type difference_left_type;
	typedef typename _Myleft::pointer left_pointer;
	typedef typename _Myleft::const_pointer const_left_pointer;
	typedef typename _Myleft::reference left_reference;
	typedef typename _Myleft::const_reference const_left_reference;
	typedef typename _Myleft::iterator left_iterator;
	typedef typename _Myleft::const_iterator const_left_iterator;
	typedef typename _Myleft::reverse_iterator reverse_left_iterator;
	typedef typename _Myleft::const_reverse_iterator
		const_reverse_left_iterator;
	typedef typename _Myleft::_Pairib _left_Pairib;
	typedef typename _Myleft::_Pairii _left_Pairii;
	typedef typename _Myleft::_Paircc _left_Paircc;
	typedef typename _Myleft::value_type value_left_type;

	typedef typename _Myright::value_compare value_right_compare;
	typedef typename _Myright::allocator_type allocator_right_type;
	typedef typename _Myright::size_type size_right_type;
	typedef typename _Myright::difference_type difference_right_type;
	typedef typename _Myright::pointer right_pointer;
	typedef typename _Myright::const_pointer const_right_pointer;
	typedef typename _Myright::reference right_reference;
	typedef typename _Myright::const_reference const_right_reference;
	typedef typename _Myright::iterator right_iterator;
	typedef typename _Myright::const_iterator const_right_iterator;
	typedef typename _Myright::reverse_iterator reverse_right_iterator;
	typedef typename _Myright::const_reverse_iterator
		const_reverse_right_iterator;
	typedef typename _Myright::_Pairib _right_Pairib;
	typedef typename _Myright::_Pairii _right_Pairii;
	typedef typename _Myright::_Paircc _right_Paircc;
	typedef typename _Myright::value_type value_right_type;

	multi2rmap() 
		: left(), lpred(), right(), rpred()
	{	// construct empty map from defaults
	}

	explicit multi2rmap(const left_compare& _Lpred,const right_compare& _Rpred)
		: left(_Lpred), lpred(_Lpred), right(_Rpred), rpred(_Rpred)
	{	// construct empty map from comparator
	}

	multi2rmap(const left_compare& _Lpred, const allocator_left_type& _Lal,const right_compare& _Rpred, const allocator_right_type& _Ral)
		: left(_Lpred, _Lal), lpred(_Lpred), right(_Rpred, _Ral), rpred(_Rpred)
	{	// construct empty map from comparator and allocator
	}

	template<class _Iter>
	multi2rmap(_Iter _First, _Iter _Last)
		: left(), lpred(), right(), rpred()
	{	// construct map from [_First, _Last), defaults
		for (; _First != _Last; ++_First)
			insert(*_First);
	}

	template<class _Iter>
	multi2rmap(_Iter _First, _Iter _Last,
		const left_compare& _Lpred,const right_compare& _Rpred)
		: left(_Lpred), lpred(_Lpred), right(_Rpred), rpred(_Rpred)
	{	// construct map from [_First, _Last), comparator
		for (; _First != _Last; ++_First)
			insert(*_First);
	}

	template<class _Iter>
	multi2rmap(_Iter _First, _Iter _Last,
		const left_compare& _Lpred, const allocator_left_type& _Lal,const right_compare& _Rpred, const allocator_right_type& _Ral)
		: left(_Lpred, _Lal), lpred(_Lpred), right(_Rpred, _Ral), rpred(_Rpred)
	{	// construct map from [_First, _Last), comparator, and allocator
		for (; _First != _Last; ++_First)
			insert(*_First);
	}
	

	left_iterator insert(const value_left_type& _Val)
	{	// insert a {key, mapped} value
		right.insert(value_right_type(_Val.second,_Val.first));
		return (left.insert(_Val));
	}

	left_iterator insert(const_left_iterator _Where, const value_left_type& _Val)
	{	// insert a {key, mapped} value, with hint
		right.insert(value_right_type(_Val.second,_Val.first));
		return (left.insert(_Where, _Val));
	}

	void erase(const_left_iterator _Where)
	{	// erase element at _Where
		right.erase(_Where->second);
		left.erase(_Where);
	}

	void erase(const left_type& _Keyval)
	{	// erase and count all that match _Keyval
		_left_Paircc pr = left.equal_range(_Keyval);
		for (const_left_iterator cit = pr.first; cit!=pr.second;++cit)
			right.erase(cit->second);

		(left.erase(pr.first,pr.second));
	}

	void erase(const_left_iterator _First, const_left_iterator _Last)
	{	// erase [_First, _Last)
		for (const_left_iterator cit=_First; cit != _Last; ++cit)
			right.erase(cit->second);

		left.erase(_First, _Last);
	}

	template<class _Iter>
	void insert(_Iter _First, _Iter _Last)
	{	// insert [_First, _Last), arbitrary iterators
		for (; _First != _Last; ++_First)
			insert(*_First);
	}

	void rerase(const_right_iterator _Where)
	{	// erase element at _Where
		_left_Paircc pr = left.equal_range(_Where->second);
		for (const_left_iterator cit = pr.first; cit!=pr.second;++cit)
			if (!_DEBUG_LT_PRED(rpred, _Where->first, cit->second)
				&& !_DEBUG_LT_PRED(rpred, cit->second, _Where->first)) {
				//if(_Where->first==cit->second) {
				left.erase(cit);
				break;
			}

		right.erase(_Where);
	}

	void rerase(const right_type& _Keyval)
	{	// erase and count all that match _Keyval
		rerase(right.find(_Keyval));
	}

	void rerase(const_right_iterator _First, const_right_iterator _Last)
	{	// erase [_First, _Last)
		for (; _First != _Last; ++_First)
			rerase(_First);
	}

	void clear()
	{
		left.clear();
		right.clear();
	}

public:
	_Myleft left;
	left_compare lpred;
	_Myright right;
	right_compare rpred;
};

//////////////////////////////////////////////////////////////////////////

#endif//_H_MAP_H_