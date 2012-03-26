#ifndef CROSSMAP_H
#define	CROSSMAP_H

#include <hash_map>
#include <vector>

using namespace std;
using namespace stdext;

namespace DataStructure
{
	template<class T1, class T2>
	class CrossMap
	{
	private:
		hash_map<T1, int>		_firstMap;
		hash_map<T2, int>		_secondMap;
		vector< pair<T1, T2> >	_data;

	public:
		CrossMap() {};
		CrossMap(const vector< pair<T1, T2> >& p_data) : _data(p_data)
		{
			for (int i = 0; i < (int)p_data.size(); ++i)
			{
				_firstMap[p_data[i].first] = _secondMap[p_data[i].second] = i;
			}
		}
		//----------------------------------------------------------------------------------------------
		void Clear()
		{
			_firstMap.clear();
			_secondMap.clear();
			_data.clear();
		}
		//----------------------------------------------------------------------------------------------
		void FirstValues(vector<T1>& p_values, bool p_append = false)
		{
			if (p_append)
			{
				p_values.reserve(p_values.size() + _data.size());
				for(int i = 0, size = _data.size(); i < size; ++i)
				{
					p_values.push_back(_data[i].first);
				}
			}
			else
			{
				p_values.resize(_data.size());
				for(int i = 0, size = _data.size(); i < size; ++i)
				{
					p_values[i] = _data[i].first;
				}
			}
		}
		//----------------------------------------------------------------------------------------------
		void SecondValues(vector<T2>& p_values, bool p_append = false)
		{
			if (p_append)
			{
				p_values.reserve(p_values.size() + _data.size());
				for(int i = 0, size = _data.size(); i < size; ++i)
				{
					p_values.push_back(_data[i].second);
				}
			}
			else
			{
				p_values.resize(_data.size());
				for(int i = 0, size = _data.size(); i < size; ++i)
				{
					p_values[i] = _data[i].second;
				}
			}
		}
		//----------------------------------------------------------------------------------------------
		T2 GetByFirst(const T1& p_key) 
		{
			return _data[_firstMap[p_key]].second; 
		};
		//----------------------------------------------------------------------------------------------
		T1 GetBySecond(const T2& p_key) 
		{
			return _data[_secondMap[p_key]].first; 
		};
		//----------------------------------------------------------------------------------------------
		void SetByFirst(const T1& p_first, T2& p_newSecond) 
		{
			int index;
			if(_firstMap.find(p_first) == _firstMap.end())
			{
				_data.push_back(make_pair(p_first, p_newSecond));
				index = _data.size() - 1;
				_secondMap[p_newSecond] = index;
				_firstMap[p_first] = index;
			}
			else
			{
				index = _firstMap[p_first];
				T2& oldSecond = _data[index].second;
				if(p_newSecond == oldSecond)
					return;

				_secondMap.erase(oldSecond);
				_secondMap[p_newSecond] = index;
				_data[index].second = p_newSecond;
			}
		};
		//----------------------------------------------------------------------------------------------
		void SetBySecond(const T2& p_second, T1& p_newFirst) 
		{
			int index;
			if(_secondMap.find(p_second) == _secondMap.end())
			{
				_data.push_back(make_pair(p_newFirst, p_second));
				index = _data.size() - 1;
				_secondMap[p_second] = index;
				_firstMap[p_newFirst] = index;
			}
			else
			{
				index = _secondMap[p_second];
				T1& oldFirst = _data[index].first;

				if(p_newFirst == oldFirst)
					return;

				_firstMap.erase(oldFirst);
				_firstMap[p_newFirst] = index;
				_data[index].first = p_newFirst;
			}
		};
		//----------------------------------------------------------------------------------------------
		bool ContainsFirst(const T1& p_first)
		{
			return _firstMap.find(p_first) != _firstMap.end();
		}
		//----------------------------------------------------------------------------------------------
		bool ContainsSecond(const T2& p_second)
		{
			return _secondMap.find(p_second) != _secondMap.end();
		}
		//----------------------------------------------------------------------------------------------
		bool TryGetByFirst(const T1& p_key1, T2& p_key2)
		{ 
			hash_map<T1, int>::const_iterator itr = _firstMap.find(p_key1);

			if (itr != _firstMap.end())
			{
				p_key2 = _data[(*itr).second].second;
				return true;
			}
			else
			{
				return false;
			}
		}
		//----------------------------------------------------------------------------------------------
		bool TryGetBySecond(const T2& p_key1, T1& p_key2)
		{ 
			hash_map<T1, int>::const_iterator itr = _secondMap.find(p_key1);

			if (itr != _secondMap.end())
			{
				p_key2 = _data[(*itr).second].first;
				return true;
			}
			else
			{
				return false;
			}
		}
	};
}

#endif	// CROSSMAP_H