require "prime"

class Numeric
  def divisors
    # 1. the below algorithm is too simplistic to apply to a big number
    (1..self).select { |n| (self / n) * n == self }
  end

  def divisors2
    primes, powers = self.prime_division.transpose
    exponents = powers.map { |i| (0..i).to_a }
    result = exponents.shift.product(*exponents).map { |powers|
      primes.zip(powers).map { |prime, power| prime ** power }.reduce(:*)
    }

    result.sort.map { |div| div }
  end

  def D
    divisors2[0..-2].reduce(:+)
  end
end
