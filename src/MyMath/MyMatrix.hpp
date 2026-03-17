#pragma once

#include "Assert.hpp"
#include "Types.hpp"

#include <vector>

template <typename T>
class Matrix2D
{
	std::vector<T> m_vector;
	u64 m_cols;
	u64 m_rows;

public:

	Matrix2D(u64 cols, u64 rows) :
	m_cols(cols),
	m_rows(rows),
	m_vector(cols * rows)
	{
		Assert(cols > 0 && rows > 0);
	}

	Matrix2D()
	{
		m_cols = 0;
		m_rows = 0;
	}

	T& operator[](const u64 x, const u64 y)
	{
		Assert((y * m_cols + x) < m_vector.size());

		return m_vector[y * m_cols + x];
	}

	const T& operator[](const u64 x, const u64 y) const
	{
		Assert((y * m_cols + x) < m_vector.size());

		return m_vector[y * m_cols + x];
	}

	T& operator()(const u64 x, const u64 y)
	{
		Assert((y * m_cols + x) < m_vector.size());

		return m_vector[y * m_cols + x];
	}

	const T& operator()(const u64 x, const u64 y) const
	{
		Assert((y * m_cols + x) < m_vector.size());

		return m_vector[y * m_cols + x];
	}

	typename std::vector<T>::iterator begin() // NOLINT
	{
		return m_vector.begin();
	}

	typename std::vector<T>::iterator end() // NOLINT
	{
		return m_vector.end();
	}

	typename std::vector<T>::const_iterator begin() const // NOLINT
	{
		return m_vector.begin();
	}

	typename std::vector<T>::const_iterator end() const // NOLINT
	{
		return m_vector.end();
	}

	void Set(const u64 x, const u64 y, T&& value)
	{
		(*this)(x, y) = std::forward<T>(value);
	}

	void Clear()
	{
		m_vector.clear();
		m_vector.shrink_to_fit();

		m_cols = 0;
		m_rows = 0;
	}

	void Reserve(const u64 cols, const u64 rows)
	{
		m_vector.reserve(cols * rows);
	}

	void Resize(const u64 newCols, const u64 newRows)
	{
		if (newCols <= 0 && newRows <= 0)
		{
			Clear();
			return;
		}

		if (newCols == m_cols && newRows == m_rows)
		{
			return;
		}

		std::vector<T> newVector(newCols * newRows);

		for (u64 y = 0; y < std::min(m_rows, newRows); y++)
		{
			for (u64 x = 0; x < std::min(m_cols, newCols); x++)
			{
				newVector[y * newCols + x] = (*this)(x, y);
			}
		}

		m_vector = std::move(newVector);
		m_cols = newCols;
		m_rows = newRows;
	}

	void Resize(const u64 newCols, const u64 newRows, const T& def)
	{
		if (newCols <= 0 && newRows <= 0)
		{
			Clear();
			return;
		}

		if (newCols == m_cols && newRows == m_rows)
		{
			return;
		}

		std::vector<T> newVector(newCols * newRows, def);

		for (u64 y = 0; y < std::min(m_rows, newRows); y++)
		{
			for (u64 x = 0; x < std::min(m_cols, newCols); x++)
			{
				newVector[y * newCols + x] = (*this)(x, y);
			}
		}

		m_vector = std::move(newVector);
		m_cols = newCols;
		m_rows = newRows;
	}

	void RemoveColumn(const u64 colIndex)
	{
		Assert(colIndex < m_cols);

		std::vector<T> newVector((m_cols - 1) * m_rows);

		for (u64 y = 0; y < m_rows; ++y)
		{
			u64 newX = 0;
			for (u64 x = 0; x < m_cols; ++x)
			{
				if (x != colIndex)
				{
					newVector[y * (m_cols - 1) + newX] = (*this)(x, y);
					++newX;
				}
			}
		}

		m_vector = std::move(newVector);

		--m_cols;
	}

	void RemoveRow(u64 rowIndex)
	{
		Assert(rowIndex < m_rows);

		std::vector<T> newVector(m_cols * (m_rows - 1));

		u64 newY = 0;
		for (u64 y = 0; y < m_rows; ++y)
		{
			if (y != rowIndex)
			{
				for (u64 x = 0; x < m_cols; ++x)
				{
					newVector[newY * m_cols + x] = (*this)(x, y);
				}
			}
			++newY;
		}

		m_vector = std::move(newVector);

		--m_rows;
	}

	void InsertColumn(const u64 colIndex)
	{
		Assert(colIndex <= m_cols);

		std::vector<T> newVector((m_cols + 1) * m_rows);

		for (u64 y = 0; y < m_rows; ++y)
		{
			u64 newX = 0;
			for (u64 x = 0; x < m_cols + 1; ++x)
			{
				if (x != colIndex)
				{
					u64 oldX = (x < colIndex) ? x : x - 1;
					newVector[y * (m_cols + 1) + newX] = (*this)(oldX, y);
				}

				else
				{
					++newX;
				}
			}
		}

		m_vector = std::move(newVector);

		++m_cols;
	}

	std::vector<T> Data() const
	{
		return m_vector;
	}

	u64 GetCols() const
	{
		return m_cols;
	}
	u64 GetRows() const
	{
		return m_rows;
	}
};
