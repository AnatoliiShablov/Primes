#ifndef PRIMES_H
#define PRIMES_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

#ifdef DEBUG_MODE
#include <chrono>
#include <iostream>
#endif

namespace {
/**
 * @brief Размер сектора, на котором вычисляются значения изначально.
 */
const uint32_t FIRST_SECTOR{2048};
/**
 * @brief Размер сектора, на котором вычисляются значения при вызове \link
 * PrimesCache::add_primes() \endlink.
 */
const uint32_t SECTOR_SIZE{1048576};
/**
 * @brief Первое число, квадрат которого выходит за границы UINT32_MAX.
 */
const uint32_t UINT32_MAX_SQRT{65536};
} // namespace

/**
 * @brief Класс для вычисления и хранения простых чисел
 */
class PrimesCache {
public:
  /**
   * @brief Тип итератора для \link PrimesCache \endlink.
   */
  using const_iterator = std::vector<uint32_t>::const_iterator;

  /**
   * @brief Конструктор.
   *
   * При создании объекта находит все простые числа до FIRST_SECTOR.
   */
  PrimesCache();

  /**
   * @brief Функция генерации новых чисел.
   *
   * При вызове находит все простые числа от \link PrimesCache::last_checked()
   * \endlink  до \link PrimesCache::last_checked() \endlink + SECTOR_SIZE.
   */
  void add_primes();

  /**
   * В случае если простых чисел в уже сгенерированном массиве данных
   * недостаточно генерирует новые.
   * @param pos
   * @return Простое число на позии pos в случае успеха, иначе 0.
   */
  uint32_t operator[](uint32_t pos);
  /**
   * В случае если простых чисел в уже сгенерированном массиве данных
   * недостаточно ищет новые не выделяя при этом дополнительной памяти. После
   * возврата найденного числа не запоминает его.
   * @param pos
   * @return Простое число на позии pos в случае успеха, иначе 0.
   */
  uint32_t operator()(uint32_t pos) const noexcept;

  /**
   * @return Итератор на начало контейнера.
   */
  const_iterator begin() const noexcept;
  /**
   * @return Итератор на конец контейнера.
   */
  const_iterator end() const noexcept;

  /**
   * @return Последнее проверенное на простоту число внутри функции \link
   * PrimesCache::add_primes() \endlink.
   */
  uint32_t last_checked() const noexcept;
  /**
   * @return Количество найденных и запомненных простых чисел.
   */
  uint32_t size() const noexcept;

private:
  std::vector<uint32_t> data_;
  uint32_t last_checked_;
};

/**
 * @brief Класс для вычисления и хранения простых чисел.
 *
 * Оптимизирован по памяти, позволяет ограничить доступ к поиску больших чисел
 */
class Primes {
public:
  /**
   * @brief Конструктор контейнера без верхней границы.
   */
  Primes();
  /**
   * @brief Конструктор контейнера с верхней границей.
   */
  Primes(uint32_t max_value);

  /**
   * В случае если простых чисел в уже сгенерированном массиве данных
   * недостаточно генерирует новые.
   * @param pos
   * @return Простое число на позии pos в случае успеха, иначе 0.
   */
  uint32_t operator[](uint32_t pos);
  /**
   * В случае если простых чисел в уже сгенерированном массиве данных
   * недостаточно ищет новые не выделяя при этом дополнительной памяти. После
   * возврата найденного числа не запоминает его.
   * @param pos
   * @return Простое число на позии pos в случае успеха, иначе 0.
   */
  uint32_t operator()(uint32_t pos) const noexcept;

  /**
   * @return В случае контейнера с верхней границей - количество простых чисел
   * не превыщающих заданный параметр, иначе число уже найденных простых чисел.
   */
  uint32_t size() const noexcept;

  /**
   * @brief Класс-итератор для \link Primes \endlink.
   */
  class Iterator {
  public:
    /**
     * @brief Тип разницы между итераторами для \link Primes \endlink.
     */
    using difference_type = int32_t;
    /**
     * @brief Тип значения по итератору для \link Primes \endlink.
     */
    using value_type = uint32_t;
    /**
     * @brief Тип указателя на значение по итератору для \link Primes \endlink.
     */
    using pointer = const uint32_t *;
    /**
     * @brief Тип ссылки на значение по итератору для \link Primes \endlink.
     */
    using reference = const uint32_t &;
    /**
     * @brief Вид итератора для \link Primes \endlink.
     */
    using iterator_category = std::random_access_iterator_tag;

    /**
     * @brief Конструктор.
     * @param owner
     * @param pos
     * @param end_it
     *
     * Создает итератор по классу owner на позицию pos, в случае если итератор
     * является end для контейнера без верхней границы end_it должен быть
     * установлен как true.
     */
    explicit Iterator(Primes *owner, uint32_t pos = 0,
                      bool end_it = false) noexcept;

    /**
     * @param diff
     * @return Итератор на позицию pos + diff.
     */
    Iterator &operator+=(difference_type diff) noexcept;
    /**
     * @return Итератор на позицию pos + 1.
     */
    Iterator &operator++() noexcept;
    /**
     * @return Итератор на позицию pos.
     */
    Iterator operator++(int) const noexcept;
    /**
     * @param it
     * @param diff
     * @return Итератор на позицию it.pos + diff.
     */
    friend Iterator operator+(Iterator const &it,
                              difference_type diff) noexcept;
    /**
     * @param diff
     * @param it
     * @return Итератор на позицию it.pos + diff.
     */
    friend Iterator operator+(difference_type diff,
                              Iterator const &it) noexcept;
    /**
     * @param diff
     * @return Итератор на позицию pos - diff.
     */
    Iterator &operator-=(difference_type diff) noexcept;
    /**
     * @return Итератор на позицию pos - 1.
     */
    Iterator &operator--() noexcept;
    /**
     * @return Итератор на позицию pos.
     */
    Iterator operator--(int) const noexcept;
    /**
     * @return Итератор на позицию it.pos - diff.
     */
    friend Iterator operator-(Iterator const &it,
                              difference_type diff) noexcept;
    /**
     * @param lhs
     * @param rhs
     * @return Разницу между позициями на которые указывают итераторы.
     */
    friend difference_type operator-(Iterator const &lhs,
                                     Iterator const &rhs) noexcept;

    /**
     * @param lhs
     * @param rhs
     * @return true если позиции на которые указывают итераторы равны, false -
     * иначе.
     */
    friend bool operator==(Iterator const &lhs, Iterator const &rhs) noexcept;
    /**
     * @param lhs
     * @param rhs
     * @return false если позиции на которые указывают итераторы равны, true -
     * иначе.
     */
    friend bool operator!=(Iterator const &lhs, Iterator const &rhs) noexcept;
    /**
     * @param lhs
     * @param rhs
     * @return true если позиция на которую указывает lhs меньше позиции на
     * которую указывает rhs, false - иначе.
     */
    friend bool operator<(Iterator const &lhs, Iterator const &rhs) noexcept;
    /**
     * @param lhs
     * @param rhs
     * @return true если позиция на которую указывает lhs больше позиции на
     * которую указывает rhs, false - иначе.
     */
    friend bool operator>(Iterator const &lhs, Iterator const &rhs) noexcept;
    /**
     * @param lhs
     * @param rhs
     * @return true если позиция на которую указывает lhs не меньше позиции на
     * которую указывает rhs, false - иначе.
     */
    friend bool operator>=(Iterator const &lhs, Iterator const &rhs) noexcept;
    /**
     * @param lhs
     * @param rhs
     * @return true если позиция на которую указывает lhs не больше позиции на
     * которую указывает rhs, false - иначе.
     */
    friend bool operator<=(Iterator const &lhs, Iterator const &rhs) noexcept;

    /**
     * @return Значение на позиции, на которую указывает итератор, в случае
     * успеха, иначе 0.
     */
    uint32_t operator*() const;

  private:
    Primes *owner_;
    uint32_t pos_;
    bool end_it_;
  };
  friend Iterator operator+(Iterator const &it,
                            Iterator::difference_type diff) noexcept;
  friend Iterator operator+(Iterator::difference_type diff,
                            Iterator const &it) noexcept;
  friend Iterator operator-(Iterator const &it,
                            Iterator::difference_type diff) noexcept;

  friend Iterator::difference_type operator-(Iterator const &lhs,
                                             Iterator const &rhs) noexcept;

  friend bool operator==(Iterator const &lhs, Iterator const &rhs) noexcept;
  friend bool operator!=(Iterator const &lhs, Iterator const &rhs) noexcept;

  friend bool operator<(Iterator const &lhs, Iterator const &rhs) noexcept;
  friend bool operator>(Iterator const &lhs, Iterator const &rhs) noexcept;
  friend bool operator>=(Iterator const &lhs, Iterator const &rhs) noexcept;
  friend bool operator<=(Iterator const &lhs, Iterator const &rhs) noexcept;

  /**
   * @return  Итератор на начало контейнера.
   */
  Iterator begin();
  /**
   * @return  Итератор на конец контейнера.
   */
  Iterator end();

private:
  static PrimesCache data_;
  uint32_t size_;
  bool unbound_;
};

#endif // PRIMES_H
