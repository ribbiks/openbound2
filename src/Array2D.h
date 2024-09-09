#pragma once

#include <vector>
#include <stdexcept>

template <typename T>
class Array2D {
private:
    std::vector<std::vector<T>> data;
    int rows;
    int cols;

public:
    Array2D() : rows(0), cols(0) {}

    Array2D(int rows, int cols, const std::vector<T>& original) 
        : rows(rows), cols(cols) {
        if (rows * cols != original.size()) {
            throw std::invalid_argument("Input vector size does not match the specified dimensions");
        }
        data.resize(rows);
        for (int i = 0; i < rows; ++i) {
            data[i].resize(cols);
            for (int j = 0; j < cols; ++j) {
                data[i][j] = original[i + j * rows];
            }
        }
    }

    Array2D(int rows, int cols, const T& initial_value) 
        : rows(rows), cols(cols) {
        data.resize(rows);
        for (int i = 0; i < rows; ++i) {
            data[i].resize(cols, initial_value);
        }
    }

    std::vector<T>& operator[](int index) {
        if (index < 0 || index >= rows) {
            throw std::out_of_range("Row index out of range");
        }
        return data[index];
    }

    const std::vector<T>& operator[](int index) const {
        if (index < 0 || index >= rows) {
            throw std::out_of_range("Row index out of range");
        }
        return data[index];
    }

    int width() const {
        return rows;
    }

    int height() const {
        return cols;
    }
};
