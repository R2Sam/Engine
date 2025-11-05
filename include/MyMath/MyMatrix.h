#pragma once

#include "Core/Types.h"

#include <vector>
#include <cassert>

template<typename T>
class Matrix2D
{
    std::vector<T> vector;
    u64 cols;
    u64 rows;

public:

    Matrix2D(u64 cols, u64 rows) :
    cols(cols),
    rows(rows),
    vector(cols * rows)
    {
        assert(cols > 0 && rows > 0);
    }

    Matrix2D()
    {
        cols = 0;
        rows = 0;
    }

    T& operator[](const u64 x, const u64 y)
    {
        assert((y * cols + x) < vector.size());

        return vector[y * cols + x];
    }

    const T& operator[](const u64 x, const u64 y) const
    {
        assert((y * cols + x) < vector.size());

        return vector[y * cols + x];
    }

    T& operator()(const u64 x, const u64 y)
    {
        assert((y * cols + x) < vector.size());

        return vector[y * cols + x];
    }

    const T& operator()(const u64 x, const u64 y) const
    {
        assert((y * cols + x) < vector.size());

        return vector[y * cols + x];
    }

    typename std::vector<T>::iterator begin()
    {
        return vector.begin();
    }

    typename std::vector<T>::iterator end()
    {
        return vector.end();
    }

    typename std::vector<T>::const_iterator begin() const
    {
        return vector.begin();
    }

    typename std::vector<T>::const_iterator end() const
    {
        return vector.end();
    }

    void set(const u64 x, const u64 y, T&& value)
    {
        (*this)(x, y) = std::forward<T>(value);
    }

    void clear()
    {
        vector.clear();
        vector.shrink_to_fit();
        
        cols = 0;
        rows = 0;
    }

    void reserve(const u64 cols, const u64 rows)
    {
        vector.reserve(cols * rows);
    }

    void resize(const u64 newCols, const u64 newRows)
    {
        if (newCols <= 0 && newRows <= 0)
        {
            clear();
            return;
        }

        if (newCols == cols && newRows == rows)
        {
            return;
        }

        std::vector<T> newVector(newCols * newRows);

        for (u64 y = 0; y < std::min(rows, newRows); y++)
        {
            for (u64 x = 0; x < std::min(cols, newCols); x++)
            {
                newVector[y * newCols + x] = (*this)(x, y);
            }
        }

        vector = std::move(newVector);
        cols = newCols;
        rows = newRows;
    }

    void resize(const u64 newCols, const u64 newRows, const T& def)
    {
        if (newCols <= 0 && newRows <= 0)
        {
            clear();
            return;
        }

        if (newCols == cols && newRows == rows)
        {
            return;
        }

        std::vector<T> newVector(newCols * newRows, def);

        for (u64 y = 0; y < std::min(rows, newRows); y++)
        {
            for (u64 x = 0; x < std::min(cols, newCols); x++)
            {
                newVector[y * newCols + x] = (*this)(x, y);
            }
        }

        vector = std::move(newVector);
        cols = newCols;
        rows = newRows;
    }

    void removeColumn(const u64 colIndex)
    {
        assert(colIndex < cols);

        std::vector<T> newVector((cols - 1) * rows);

        for (u64 y = 0; y < rows; ++y)
        {
            u64 newX = 0;
            for (u64 x = 0; x < cols; ++x)
            {
                if (x != colIndex)
                {
                    newVector[y * (cols - 1) + newX] = (*this)(x, y);
                    ++newX;
                }
            }
        }

        vector = std::move(newVector);

        --cols;
    }

    void removeRow(u64 rowIndex)
    {
        assert(rowIndex < rows);

        std::vector<T> newVector(cols * (rows - 1));

        u64 newY = 0;
        for (u64 y = 0; y < rows; ++y)
        {
            if (y != rowIndex)
            {
                for (u64 x = 0; x < cols; ++x)
                {
                    newVector[newY * cols + x] = (*this)(x, y);
                }
            }
            ++newY;         
        }

        vector = std::move(newVector);
        
        --rows;
    }

    void insertColumn(const u64 colIndex)
    {
        assert(colIndex <= cols);

        std::vector<T> newVector((cols + 1) * rows);

        for (u64 y = 0; y < rows; ++y)
        {
            u64 newX = 0;
            for (u64 x = 0; x < cols + 1; ++x)
            {
                if (x != colIndex)
                {
                    u64 oldX = (x < colIndex) ? x : x - 1;
                    newVector[y * (cols + 1) + newX] = (*this)(oldX, y);
                }

                else
                {
                    ++newX;
                }
            }
        }

        vector = std::move(newVector);

        ++cols;
    }

    std::vector<T> data() const
    {
        return vector;
    }

    u64 getCols() const { return cols; }
    u64 getRows() const { return rows; }
};
