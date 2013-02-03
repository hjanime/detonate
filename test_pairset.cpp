#define BOOST_TEST_MODULE test_pairset
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "pairset.hh"

typedef boost::mpl::list<brute_force_pairset, big_matrix_pairset, smart_pairset> test_types;
//typedef boost::mpl::list<brute_force_pairset, big_matrix_pairset> test_types;

size_t choose_2(size_t n) { return n*(n+1)/2; }

static size_t no_exceptions[0] = {};

BOOST_AUTO_TEST_CASE_TEMPLATE(simple, PairsetType, test_types)
{
  PairsetType ps(5);
  BOOST_CHECK_EQUAL(ps.size(), 0ul);

  size_t one_exception[1] = {1};
  ps.add_square_with_exceptions(1, 1, one_exception, one_exception+1);
  BOOST_CHECK_EQUAL(ps.size(), 0ul);

  ps.add_square_with_exceptions(1, 1, no_exceptions, no_exceptions);
  BOOST_CHECK_EQUAL(ps.size(), 1ul);

  ps.add_square_with_exceptions(1, 1, one_exception, one_exception+1);
  BOOST_CHECK_EQUAL(ps.size(), 1ul);

  ps.add_square_with_exceptions(0, 4, no_exceptions, no_exceptions);
  BOOST_CHECK_EQUAL(ps.size(), choose_2(5));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(crossovers, PairsetType, test_types)
{
  PairsetType ps(100);
  BOOST_CHECK_EQUAL(ps.size(), 0ul);

  ps.add_square_with_exceptions(0, 50-1, no_exceptions, no_exceptions);
  BOOST_CHECK_EQUAL(ps.size(), choose_2(50));

  ps.add_square_with_exceptions(60, 100-1, no_exceptions, no_exceptions);
  BOOST_CHECK_EQUAL(ps.size(), choose_2(50) + choose_2(40));

  ps.add_square_with_exceptions(40, 65-1, no_exceptions, no_exceptions);
  BOOST_CHECK_EQUAL(ps.size(), choose_2(50) + choose_2(40) + choose_2(25) - choose_2(10) - choose_2(5));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(random_squares, PairsetType, test_types)
{
  size_t num_tests = 100;
  size_t max_num_squares = 50;
  size_t max_num_exceptions = 25;
  size_t top = 100;

  boost::random::random_device rng;
  boost::random::uniform_int_distribution<> lo_hi_dist(0, top - 1);
  boost::random::uniform_int_distribution<> num_squares_dist(0, max_num_squares);
  boost::random::uniform_int_distribution<> num_exceptions_dist(0, max_num_exceptions);

  for (size_t test = 0; test < num_tests; ++test) {

    PairsetType ps(top);
    BOOST_CHECK_EQUAL(ps.size(), 0ul);

    brute_force_pairset bf(top);
    BOOST_CHECK_EQUAL(bf.size(), 0ul);

    size_t num_squares = num_squares_dist(rng);
    for (size_t sq = 0; sq < num_squares; ++sq) {
      size_t lo = lo_hi_dist(rng);
      size_t hi = lo_hi_dist(rng);
      if (lo >= hi)
        std::swap(lo, hi);

      size_t num_exceptions = num_exceptions_dist(rng);
      boost::random::uniform_int_distribution<> exceptions_dist(lo, hi);
      std::set<size_t> exceptions;
      for (size_t i = 0; i < num_exceptions; ++i)
        exceptions.insert(exceptions_dist(rng));

      /*
      std::cout << "Adding square lo=" << lo << ", hi=" << hi << ", exceptions=";
      BOOST_FOREACH(size_t x, exceptions)
        std::cout << x << " ";
      std::cout << std::endl;
      */
        
      ps.add_square_with_exceptions(lo, hi, exceptions.begin(), exceptions.end());
      bf.add_square_with_exceptions(lo, hi, exceptions.begin(), exceptions.end());

      BOOST_CHECK_EQUAL(ps.size(), bf.size());
    }

  }
}
