program main
!
!*******************************************************************************
!
!! MAIN is the main program for DIEHARD.
!
!
!  Suggestions:
!
!    double precision -> real ( kind = 8 )
!
  implicit none
!
  character ( len = 80 ) filename
  character ( len = 80 ) fileout
!
  call timestamp ( )
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'DIEHARD'
  write ( *, '(a)' ) '  Randomness tests for a file of 32-bit integers.'

  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) ' Enter input filename'
  read ( *, '(a)' ) filename
!
!  Open a file on unit 1 from which the random number data can be read.
!
  open ( 1, file = filename, form = 'unformatted', access = 'direct', &
    recl = 16384 )

  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) ' Enter name of output file:'
  read ( *, '(a)' ) fileout
  open ( 3, file = fileout )

  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) '  HERE ARE THE TESTS:'
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) '    Birthday Spacings'
  write ( *, '(a)' ) '    Overlapping Permutations'
  write ( *, '(a)' ) '    The 31x31 Matrix Rank Test'
  write ( *, '(a)' ) '    The 32x32 Matrix Rank Test'
  write ( *, '(a)' ) '    The 6x8 Matrix Rank Test'
  write ( *, '(a)' ) '    The 20-bit Word Monkey Tests'
  write ( *, '(a)' ) '    The OPSO, OQSO, DNA Monkey Tests'
  write ( *, '(a)' ) '    Count the 1''s in a Stream of Bytes'
  write ( *, '(a)' ) '    Count the 1''s in Specific Bytes'
  write ( *, '(a)' ) '    Parking Lot Test'
  write ( *, '(a)' ) '    Minimum Distance Test'
  write ( *, '(a)' ) '    Random 3D Spheres Test'
  write ( *, '(a)' ) '    The Squeeze Test'
  write ( *, '(a)' ) '    Overlapping Sums Test'
  write ( *, '(a)' ) '    Runs Up and Down Test'
  write ( *, '(a)' ) '    The Craps Test'
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'NOTE: Most of the tests in DIEHARD return a '
  write ( *, '(a)' ) 'p-value, which should be uniform on [0,1) if '
  write ( *, '(a)' ) 'the input file contains truly independent '
  write ( *, '(a)' ) 'random bits.   Those p-values are obtained '
  write ( *, '(a)' ) 'by p=F(X), where F is the assumed distribution '
  write ( *, '(a)' ) 'of the sample random variable X---often normal. '
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'But that assumed F is just an asymptotic '
  write ( *, '(a)' ) 'approximation, for which the fit will be worst'
  write ( *, '(a)' ) 'in the tails.  Thus you should not be surprised '
  write ( *, '(a)' ) 'with occasional p-values near 0 or 1, such as '
  write ( *, '(a)' ) '0.0012 or 0.9983.'
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'When a bit stream really FAILS BIG, you will '
  write ( *, '(a)' ) 'get p''s of 0 or 1 to six or more places.  '
  write ( *, '(a)' ) 'By all means, do not, as a Statistician might, '
  write ( *, '(a)' ) 'think that a p < 0.025 or 0.975 < p means'
  write ( *, '(a)' ) 'that the RNG has "failed the test at the '
  write ( *, '(a)' ) '0.05 level".  Such p''s happen among the '
  write ( *, '(a)' ) 'hundreds that DIEHARD produces, even with '
  write ( *, '(a)' ) 'good RNG''s.  So keep in mind that " p happens".'

  call birthday_test ( filename )

  call overlap_perm_test ( filename )
  
  call rank31_test ( filename )

  call rank32_test ( filename )

  call bin_rank_test ( filename )

  call bitstream_test ( filename )

  call omso ( filename )

  call count_ones_a_test ( filename )

  call count_ones_b_test ( filename )

  call parking_test ( filename )

  call min_dist_test ( filename )

  call spheres_test ( filename )

  call squeeze_test ( filename )

  call overlap_sum_test ( filename )

  call runs_test ( filename )

  call craps_test ( filename )

  write ( 3, 472 ) fileout
 472  format(' Results of DIEHARD battery of tests sent to file ', a15)

  close ( unit = 1 )

  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'DIEHARD'
  write ( *, '(a)' ) '  Normal end of execution.'

  write ( *, '(a)' ) ' '
  call timestamp ( )

  stop
end
subroutine bin_rank_test ( filename )
!
!*******************************************************************************
!
!! BIN_RANK_TEST applies the binary rank test.
!
!
!  Discussion:
!
!    The test computes the ranks of 100,000 6x8 binary matrices
!    Each row is a byte from a RNG, with overlapping rows.
!
  implicit none
!
  real e
  character ( len = * ) filename
  integer i
  integer i8bit
  integer ij
  integer j
  integer jkk
  integer jkreset
  integer jtbl
  integer k(2:6)
  integer kr
  integer l
  integer mr
  real, dimension(2:6) :: p = (/ &
    0.149858E-06, 0.808926E-04, 0.936197E-02, 0.217439E+00, 0.773118E+00 /)
  real pks
  real pp(0:25)
  integer r(6)
  integer rankb
  real s
  real t

  character*6 rk(4:6)

  data rk / 'r <= 4',' r = 5 ',' r = 6'/
!
!  rank 2 to 6 with prob p(2),...,p(6); 2,3,4 pooled.
!  one-line function to get random byte:
!
   i8bit() = and ( rshift ( jtbl(), kr ), 255 )

  write ( 3, '(a)'      ) ' '
  write ( 3, '(a)'      ) 'The BINARY RANK TEST for 6x8 matrices.'
  write ( 3, '(a)'      ) ' '
  write ( 3, '(a)'      ) 'From each of six random 32-bit integers from '
  write ( 3, '(a)'      ) 'the generator under test, a specified byte '
  write ( 3, '(a)'      ) 'is chosen.  The resulting six bytes form a'
  write ( 3, '(a)'      ) '6x8 binary matrix whose rank is determined.  '
  write ( 3, '(a)'      ) 'That rank can be from 0 to 6, but ranks '
  write ( 3, '(a)'      ) '0,1,2,3 are rare; their counts are pooled '
  write ( 3, '(a)'      ) 'with those for rank 4. '
  write ( 3, '(a)'      ) ' '
  write ( 3, '(a)'      ) 'Ranks are found for 100,000 random matrices, '
  write ( 3, '(a)'      ) 'and a chi-square test is performed on counts '
  write ( 3, '(a)'      ) 'for ranks 6,5 and <=4.'

   write(3,37) filename
37     format('         Binary Rank Test for ',a15)
371    format('        Rank of a 6x8 binary matrix,',/, &
     '     rows formed from eight bits of the RNG ',a15)

  do ij = 25, 1, -1

    jkk = jkreset()
    kr = ij - 1
    k(2:6) = 0

    write(3,371) filename
    write(3,27) 25-kr,32-kr
27  format('     b-rank test for bits ',i2,' to ',i2)
    write(3,372)
372 format(15x,'      OBSERVED   EXPECTED     (O-E)^2/E      SUM')

    do l = 1, 100000

      do i = 1, 6
        r(i) = i8bit()
      end do

      mr = rankb ( 6, 8, r )

      mr = max ( mr, 4 )

      k(mr) = k(mr) + 1

    end do

    s = 0.0

    do l = 4, 6

      if ( 4 < l ) then
        e = 100000.0E+00 * p(l)
      else
        e = 100000.0E+00 * ( p(2) + p(3) + p(4) )
      end if

      t = ( k(l) - e )**2 / e
      s = s + t
      write(3,29) rk(L),k(L),E,(k(L)-E)**2/E,s

    end do

29  format(6x,a9,i12,f12.1,f12.3,f12.3)

    pp(kr) = 1.0 - exp ( - s / 2.0 )

    write(3,23) pp(kr)

    jkk = jkreset()

  end do

23     format(12x,'            p = 1-exp(-SUM/2)=',f7.5)
   write(3,373)
373   format('   TEST SUMMARY, 25 tests on 100,000 random 6x8 matrices' &
     ,/,' These should be 25 uniform [0,1] random variables:')

   write(3,21) (pp(i),i = 24,0,-1)
21     format(5f12.6)

  call rvec_sort_heap_a ( 25, pp(0) )

    write(3,2345) filename
2345    format('   brank test summary for ',a15,/, &
     '       The KS test for those 25 supposed UNI''s yields')

  call ks_test ( 25, pp(0), pks )

  write ( 3, '(a,f8.6)' ) '  P-value from KS test for 25 tests = ', pks

  return
end
subroutine birthday_test ( filename )
!
!*******************************************************************************
!
!! BIRTHDAY_TEST applies the birthday spacings test.
!
  implicit none
!
  real alam
  integer b(4096)
  integer c(4096)
  character ( len = * ) filename
  integer i
  integer inbits
  integer j
  integer jb
  integer jkk
  integer jkreset
  integer jtbl
  integer kr
  integer l
  integer lk
  integer, parameter :: m = 512
  integer mask
  integer mspace(1000)
  integer, parameter :: nbits = 24
  integer, parameter :: nsampl = 500
  real pks(64)
  real pp
  integer s

  inbits() = and ( rshift ( jb, kr ), mask )

  write ( 3, '(a)'      ) ' '
  write ( 3, '(a)'      ) 'This is the BIRTHDAY SPACINGS TEST'
  write ( 3, '(a)'      ) ' '
  write ( 3, '(a)'      ) 'Choose m birthdays in a year of n days.  '
  write ( 3, '(a)'      ) 'List the spacings between the birthdays.  '
  write ( 3, '(a)'      ) 'If j is the number of values that occur '
  write ( 3, '(a)'      ) 'more than once in that list, then j is '
  write ( 3, '(a)'      ) 'asymptotically Poisson distributed with '
  write ( 3, '(a)'      ) 'mean m^3/(4n).  '
  write ( 3, '(a)'      ) ' '
  write ( 3, '(a)'      ) 'Experience shows n must be quite large, '
  write ( 3, '(a)'      ) 'say n>=2^18, for comparing the results '
  write ( 3, '(a)'      ) 'to the Poisson distribution with that mean.  '
  write ( 3, '(a)'      ) 'This test uses n=2^24 and m=2^9, so that '
  write ( 3, '(a)'      ) 'the underlying distribution for j is taken '
  write ( 3, '(a)'      ) 'to be Poisson with lambda=2^27/(2^26)=2.  '
  write ( 3, '(a)'      ) ' '
  write ( 3, '(a)'      ) 'A sample of 500 j''s is taken, and a chi-square '
  write ( 3, '(a)'      ) 'goodness of fit test provides a p value.  '
  write ( 3, '(a)'      ) 'The first test uses bits 1-24 (counting from '
  write ( 3, '(a)'      ) 'the left) from integers in the specified file.'
  write ( 3, '(a)'      ) ' '
  write ( 3, '(a)'      ) 'Then the file is closed and reopened. Next, '
  write ( 3, '(a)'      ) 'bits 2-25 are used to provide birthdays, '
  write ( 3, '(a)'      ) 'then 3-26 and so on to bits 9-32.  Each set '
  write ( 3, '(a)'      ) 'of bits provides a p-value, and the nine '
  write ( 3, '(a)'      ) 'p-values provide a sample for a KS test.' 

  alam = real ( m**3 ) / 2.0E+00**( nbits + 2 )

  write(3,22) m, NBITS, ALAM
  write(3,2345) filename
2345  format('           Results for ',a15)
22    FORMAT(' BIRTHDAY SPACINGS TEST, M = ',I4,' N=2**',I2,' LAMBDA=',F8.4)

  mask = 2**(nbits-1) + 2**(nbits-1) - 1

  do kr = 32-nbits, 0, -1

    s = 0
    jkk = jkreset()

    do j = 1, nsampl

      do i = 1, m
        jb = jtbl()
        b(i) = inbits()
      end do

      call ivec_sort_heap_a ( m, b )

      c(1) = b(1)
      c(2:m) = b(2:m) - b(1:m-1)

      call ivec_sort_heap_a ( m, c )

      l = 0

      do i = 2, m
        lk = 0
        if ( c(i)  == c(i-1) ) then
          lk = lk + 1
          l = l + 1
        end if
      end do

      s = s + l
      mspace(j) = l

    end do

    write(3,432) nsampl
    write(3,331) filename,33-nbits-kr,32-kr, real ( s ) / nsampl
331     format(10x,a16,' using bits ',i2,' to ',i2,f8.3,f10.6)
432     format(17x,'  For a sample of size',i4,':     mean   ')

    call chsqts ( alam, mspace, nsampl, pp )

    pks(9-kr) = pp
    jkk = jkreset()

  end do

  write(3,652) pks(1:9)
652   format('   The 9 p-values were',/,f15.6,4f10.6,/,f15.6,4f10.6)

  call ks_test ( 9, pks, pp )

  write ( 3, '(a,f8.6)' ) '  P-value from KS test for 9 tests = ', pp

  return
end
subroutine bitstream_test ( filename )
!
!*******************************************************************************
!
!! BITSTREAM_TEST carries out the monkey tests on 20 bit words.
!
!
!  Discussion:
!
!    THE OVERLAPPING 20-tuples TEST  BITSTREAM, 20 bits per word, N words
!    If n = 2^22, should be 19205.3 missing 20-letter words, sigma = 167.
!    If n = 2^21, should be 141909 missing 20-letter words, sigma = 428.
!    If n = 2^20, should be 385750 missing 20-letter words, sigma = 512
!
  implicit none
!
  character ( len = * ) filename
  integer i
  integer ib
  integer ic
  integer j
  integer jkk
  integer jkreset
  integer jtbl
  integer k
  integer kount
  integer, parameter :: kpow = 21
  integer l
  integer mbit(0:31)
  real mu
  integer, parameter :: n = 20
  integer nt
  integer, parameter :: ntries = 20
  integer num
  real phi
  real, parameter :: sigma = 428.0E+00
  integer w(0:32767)
  real x

  jkk = jkreset()

  write ( 3, '(a)'      ) ' '
  write ( 3, '(a)'      ) 'THE BITSTREAM TEST '
  write ( 3, '(a)'      ) ' '
  write ( 3, '(a)'      ) 'The file under test is viewed as a stream of '
  write ( 3, '(a)'      ) 'bits. Call them b1,b2,... .  Consider an '
  write ( 3, '(a)'      ) 'alphabet with two "letters", 0 and 1,'
  write ( 3, '(a)'      ) 'and think of the stream of bits as a '
  write ( 3, '(a)'      ) 'succession of 20-letter "words", overlapping.'
  write ( 3, '(a)'      ) ' '
  write ( 3, '(a)'      ) 'Thus the first word is b1b2...b20, the'
  write ( 3, '(a)'      ) 'second is b2b3...b21, and so on.  The '
  write ( 3, '(a)'      ) 'bitstream test counts the number of missing '
  write ( 3, '(a)'      ) '20-letter (20-bit) words in a string of'
  write ( 3, '(a)'      ) '2^21 overlapping 20-letter words.  There '
  write ( 3, '(a)'      ) 'are 2^20 possible 20 letter words.  '
  write ( 3, '(a)'      ) ' '
  write ( 3, '(a)'      ) 'For a truly random string of 2^21+19 bits, '
  write ( 3, '(a)'      ) 'the number of missing words j should be '
  write ( 3, '(a)'      ) '(very close to) normally distributed with '
  write ( 3, '(a)'      ) 'mean 141,909 and sigma 428.  Thus '
  write ( 3, '(a)'      ) '(j-141909)/428 should be a standard normal '
  write ( 3, '(a)'      ) 'variate (z score) that leads to a uniform '
  write ( 3, '(a)'      ) '[0,1) p value.  The test is repeated '
  write ( 3, '(a)'      ) 'twenty times.'
!
!  Set mask bits.
!
  mbit(0) = 1
  do i = 1, 31
    mbit(i) = 2 * mbit(i-1)
  end do
!
!  Initialize.
!
  mu = 2.0**20 * exp ( - 2.0**( kpow - 20 ) )

  write ( 3, '(a)'      ) ' '
  write ( 3, '(a,f9.0)' ) '  The number of missing words should average ', mu
  write ( 3, '(a,f4.0)' ) '  with sigma = ', sigma
!
!  Get the initial word.
!
  j = jtbl()
  j = and ( j, 2**20 - 1 )

  write(3,2345) filename
 2345 format(' BITSTREAM test results for' ,a15)

  do nt = 1, ntries

    w(0:32767) = 0
!
!  Generate 2**KPOW overlapping words.
!
    do ic = 1, 2**(kpow-5)

      num = jtbl()

      do ib = 1, 32
!
!  Get new J.
!
        j = lshift ( and ( j, 2**19 - 1 ), 1 ) + and ( num, 1 )
        num = rshift ( num, 1 )
!
!  Get bit index from last 5 bits of J.
!
        l = and ( j, 31 )
!
!  Get table index from leading 15 bits of J.
!
        k = rshift ( j, 5 )
!
!  Get bit L in W(K).
!
        w(k) = or ( w(k), mbit(l) )

      end do

    end do
!
!  Count the number of empty cells.
!
    kount = 0
    do k = 0, 32767
      do l = 0, 31
        if ( and ( w(k), mbit(l) ) == 0 ) then
          kount = kount + 1
        end if
      end do
    end do

    x = ( real ( kount ) - mu ) / sigma

    write ( 3, '(a)' ) ' '
    write ( 3, '(a,i2,a,i7)' ) &
      '  Test ', nt, &
      '  Missing words = ', kount
    write ( 3, '(a,f7.2,a,f7.5)' ) &
      '  Distance from mean in sigmas = ', x, &
      '  P-value = ', phi ( x )

  end do

  jkk = jkreset()

  return
end
function chisq ( n, x )
!
!*******************************************************************************
!
!! CHISQ ???
!
!
!  Parameters:
!
!    Input, integer N, the number of degrees of freedom.
!
!    Input, real X, the value.
!
!    Output, real CHISQ, the Chi-Squared probability.
!
  implicit none
!
  real chisq
  real d
  integer i
  integer l
  integer n
  real phi
  real s
  real t
  real x

  chisq = 0.0E+00

  if ( x <= 0.0E+00 ) then
    return
  end if

  if ( 20 < n ) then
    t = ( ( x / real ( n ) )**0.33333 - 1.0 + ( 0.22222 / real ( n ) ) ) &
      / sqrt ( 0.22222 / real ( n ) )
    t = min ( t, 8.0E+00 )
    chisq = phi ( t )
    return
  end if

  l = 4 - mod ( n, 2 )

  d = real ( min ( 1, n / 3 ) )

  do i = l, n, 2
    d = d * x / real ( i - 2 )
    chisq = chisq + d
  end do

  if ( l  == 3 ) then

    s = sqrt ( min ( 0.5 * x, 50.0 ) )

    chisq = phi ( s / 0.7071068 ) - exp ( - min ( 0.5 * x, 50.0 ) ) &
      * 0.564189 * chisq / s

  else

    chisq = 1.0 - exp ( - min ( 0.5 * x, 50.0 ) ) * ( 1.0 + chisq )

  end if

  return
end
subroutine chsqts ( lambda, mspace, nsampl, pp )
!
!*******************************************************************************
!
!! CHSQTS ???
!
!
!  Discussion:
!
!   A SUBROUTINE TO do A CHISQUARE TEST ON N VALUES FROM
!   A DISCRETE DISTRIBUTION.  SET UP FOR POISSON.  CHANGE P'S FOR OTHERS.
!
!   REQUIRES ARRAY MSPACE(NSAMPL) THAT GIVES NO. OF DUPLICATE SPACINGS
!   IN EACH OF NSAMPL YEARS.
!
  implicit none
!
  real chisq
  real ex(0:500)
  integer i
  integer j
  integer k(0:500)
  integer l
  real lambda
  integer lb
  integer lt
  integer m
  integer mspace(1000)
  integer nsampl
  real obs(0:500)
  real p
  real pp
  real ps(0:500)
  real s

  do i = 0, lambda + 4 * sqrt ( lambda )
    ex(i) = 0.0E+00
    k(i) = 0
    ps(i) = 0.0E+00
  end do

  p = exp ( -lambda )
  ps(0) = p * nsampl
  k(0) = 0
  j = 0
  s = p * real ( nsampl )

  if ( 5.0 < s ) then
    j = 1
    ex(0) = s
    s = 0.0
  end if

  do i = 1, lambda + 4 * sqrt ( lambda )

    p = lambda * p / i
    ps(i) = ps(i-1) + p * nsampl
    s = s + p * nsampl
    k(i) = j

    if ( nsampl - 5 < ps(i) ) then
      ex(j) = s + nsampl - ps(i)
      do l = i + 1, nsampl
        k(l) = j
      end do
      exit
    end if

    if ( 5.0E+00 <= s ) then
      ex(j) = s
      j = j + 1
      s = 0.0E+00
    end if

  end do

  obs(0:100) = 0.0E+00

  do i = 1, nsampl
    l = k(mspace(i))
    obs(l) = obs(l) + 1
  end do

  s = 0.0E+00
  do m = 0, j
    s = s + ( obs(m) - ex(m) )**2 / ex(m)
  end do

  lb = 0
  m = k(0)
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) ' Duplicate         Number       Number '
  write ( 3, '(a)' ) ' spacings         observed     expected'

  do i = 1, 100

    if ( k(i) == m ) then
      cycle
    end if

    lt = i-1

    if ( lb /= lt ) then
      write(3,28) lb,lt,obs(m),ex(m)
 28   format(' ',i2,' to ',i2,f13.0,f13.3)
    else
      write(3,29) lb,obs(m),ex(m)
 29   format(3x,i6,f13.0,f13.3)
    end if

    m = k(i)
    lb = i

    if ( m  == j ) then
      exit
    end if

  end do

  write(3,27) lb,obs(m),ex(m)
  27  format(' ',i2,' to INF',f12.0,f13.3)

  pp = chisq ( j, s )

  write(3,31) j,s,pp
  31  format(' Chisquare with ',i2,' d.o.f. = ',f8.2,' p-value= ',f8.6)

  return
end
subroutine count_ones_a_test ( filename )
!
!*******************************************************************************
!
!! COUNT_ONES_A_TEST applies the COUNT-THE-1'S test to a stream of bytes.
!
  implicit none
!
  real chsq
  real e
  character ( len = * ) filename
  integer i
  integer i1
  integer i2
  integer ii
  integer j
  integer jj
  integer jk
  integer jkk
  integer jkreset
  integer jtbl8
  integer kbits(0:255)
  integer ks
  integer m8
  integer, parameter :: n = 100
  integer, dimension (0:4) :: p = (/ 37, 56, 70, 56, 37 /)
  real phi
  real q4
  real q5
  integer s(0:624)
  integer t(0:3124)
  integer w
  real z
!
!  one-line function to return (truncated) no. of 1's in 8 random bits
!  that is, this function provides a random keystroke, unequal p's.
!
   m8() = kbits ( jtbl8 ( ) )

  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'COUNT-THE-1''s TEST on a stream of bytes.'
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'Consider the file under test as a stream of '
  write ( *, '(a)' ) 'bytes (four per 32 bit integer).  Each byte can '
  write ( *, '(a)' ) 'contain from 0 to 8 1''s, with probabilities '
  write ( *, '(a)' ) '1, 8, 28, 56, 70, 56, 28, 8, 1 over 256.  '
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'Now let the stream of bytes provide a string '
  write ( *, '(a)' ) 'of overlapping 5-letter words, each "letter" '
  write ( *, '(a)' ) 'taking values A, B, C, D, E. '
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'The letters are determined by the number of 1''s '
  write ( *, '(a)' ) 'in a byte::  '
  write ( *, '(a)' ) '  0, 1 or 2 --> A,'
  write ( *, '(a)' ) '  3         --> B,'
  write ( *, '(a)' ) '  4         --> C,'
  write ( *, '(a)' ) '  5         --> D and '
  write ( *, '(a)' ) '  6, 7 or 8 --> E.'
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'Thus we have a monkey at a typewriter hitting '
  write ( *, '(a)' ) 'five keys with various probabilities '
  write ( *, '(a)' ) '(37, 56, 70, 56, 37 over 256).  There are 5^5'
  write ( *, '(a)' ) 'possible 5-letter words, and from a string of '
  write ( *, '(a)' ) '256,000 (overlapping) 5-letter words, counts '
  write ( *, '(a)' ) 'are made on the frequencies for each word.   '
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'The quadratic form in the weak inverse of the '
  write ( *, '(a)' ) 'covariance matrix of the cell counts provides '
  write ( *, '(a)' ) 'a chisquare test: Q5-Q4, the difference of '
  write ( *, '(a)' ) 'the naive Pearson sums of (OBS-EXP)^2/EXP on '
  write ( *, '(a)' ) 'counts for 5- and 4-letter cell counts.'
!
!  Create kbits table: kbits(j) = truncated number of bits in j, -128<=j<=127
!  Filename reads one byte at a time, as integer*1, so -128 to 127.
!
  do jj = 0, 255

    j = jj
    ks = 0

    do i = 1, 8
      ks = ks + and ( j, 1 )
      j = rshift ( j, 1 )
    end do

    ks = max ( ks, 2 )
    ks = min ( ks, 6 )

    kbits(jj) = ks - 2

  end do

  jkk = jkreset()

  write(3,2345) filename
2345  format('   Test results for ',a15)

  do jk = 1, 2

    do i = 0, 5**4-1
      s(i) = 0
    end do

    do i = 0, 5**5-1
      t(i) = 0
    end do
!
!  Generate initial word with 5 random keystrokes.
!
    w = 5**4 * m8() + 5**3 * m8() + 5**2 * m8() + 5 * m8() + m8()

    do i2 = 1, n

      do i1 = 1, 25600
!
!  Erase leftmost letter of W.
!
        w = mod ( w, 5**4 )
!
!  Boost count for that 4-letter word.
!
        s(w) = s(w) + 1
!
!  Shift W left, add new letter, boost 5-letter word count.
!
        w = 5 * w + m8()
        t(w) = t(w) + 1

      end do

    end do
!
!  Find Q4: sum(obs-exp)^2/exp for 4-letter words.
!
    q4 = 0.0
    do ii = 0, 5**4-1
      i = ii
      e = real ( 25600 * n )
      do j = 0, 3
        e = e * p ( mod ( i, 5 ) ) / 256.0
        i = i / 5
      end do
      q4 = q4 + ( s(ii) - e )**2 / e
    end do
!
!  Find Q5: sum(obs-exp)^2/exp for 5-letter words.
!
    q5 = 0.0
    do ii = 0, 5**5-1
      i = ii
      e = real ( 25600 * n )
      do j = 0, 4
        e = e * p ( mod ( i, 5 ) ) / 256.0
        i = i / 5
      end do
      q5 = q5 + ( t(ii) - e )**2 / e
    end do

    chsq = q5 - q4
    z = ( chsq - 2500.0 ) / sqrt ( 5000.0 )

    if ( jk == 1 ) then

      write(3,819) 25600 * n
819     format(' Chi-square with 5^5-5^4 = 2500 d.of f. for sample size:', &
        i7,/,31x,'chisquare  equiv normal  p-value')
      write ( 3, '(a)' )' Results for COUNT-THE-1''s in successive bytes:'

    end if

    write(3,821) filename,chsq,z,phi(z)
821     format(' byte stream for ',a15,f9.2,f11.3,f13.6)

  end do

  jkk = jkreset()

  return
end
subroutine count_ones_b_test ( filename )
!
!*******************************************************************************
!
!! COUNT_ONES_B_TEST performs the COUNT-THE-1'S test for specific bytes.
!
  implicit none
!
  real chsq
  real e
  character ( len = * ) filename
  integer i
  integer i1
  integer i2
  integer ii
  integer j
  integer jk
  integer jkk
  integer jkreset
  integer jtbl
  integer, dimension (0:255) :: k = (/ &
     0,0,0,0,0,0,0,1,0,0,0,1,0,1,1,2,0,0,0,1,0,1,1,2,0,1,1,2,1,2,2,3, &
     0,0,0,1,0,1,1,2,0,1,1,2,1,2,2,3,0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4, &
     0,0,0,1,0,1,1,2,0,1,1,2,1,2,2,3,0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4, &
     0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,4, &
     0,0,0,1,0,1,1,2,0,1,1,2,1,2,2,3,0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4, &
     0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,4, &
     0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,4, &
     1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,4,2,3,3,4,3,4,4,4,3,4,4,4,4,4,4,4/)
  integer m8
  integer, parameter :: n = 10
  integer, dimension ( 0:4 ) :: p = (/ 37, 56, 70, 56, 37 /)
  real phi
  real q4
  real q5
  integer s(0:624)
  integer t(0:3124)
  integer w
  real z
!
!  one-line function to return no. of 1's in 8 random bits
!  that is, this function provides a random keystoke, unequal p's.
!
   m8() = k ( and ( rshift ( jtbl(), 25-jk ), 255 ) )

  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'This is the COUNT-THE-1''s TEST for specific bytes.'
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'Consider the file under test as a stream of '
  write ( 3, '(a)' ) '32-bit integers.  From each integer, a specific '
  write ( 3, '(a)' ) 'byte is chosen , say the leftmost bits 1 to 8. '
  write ( 3, '(a)' ) 'Each byte can contain from 0 to 8 1''s, with '
  write ( 3, '(a)' ) 'probabilities 1,8,28,56,70,56,28,8,1 over 256.'
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'Now let the specified bytes from successive '
  write ( 3, '(a)' ) 'integers provide a string of (overlapping) '
  write ( 3, '(a)' ) '5-letter words, each "letter" taking values'
  write ( 3, '(a)' ) 'A,B,C,D,E. The letters are determined  by the '
  write ( 3, '(a)' ) 'number of 1''s, in that byte,'
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) '  0, 1 or 2 ---> A,'
  write ( 3, '(a)' ) '  3         ---> B,'
  write ( 3, '(a)' ) '  4         ---> C,'
  write ( 3, '(a)' ) '  5         ---> D,'
  write ( 3, '(a)' ) '  6, 7 or 8 ---> E.'
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'Thus we have a monkey at a typewriter hitting '
  write ( 3, '(a)' ) 'five keys with with various probabilities'
  write ( 3, '(a)' ) '37, 56, 70, 56, 37 over 256. '
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'There are 5^5 possible 5-letter words, and'
  write ( 3, '(a)' ) 'from a string of 256,000 (overlapping) 5-letter '
  write ( 3, '(a)' ) 'words, counts are made on the frequencies for '
  write ( 3, '(a)' ) 'each word. The quadratic form in the weak '
  write ( 3, '(a)' ) 'inverse of the covariance matrix of the cell '
  write ( 3, '(a)' ) 'counts provides a chisquare test.'
  write ( 3, '(a)' ) 'Q5-Q4, the difference of the naive Pearson sums'
  write ( 3, '(a)' ) 'of (OBS-EXP)^2/EXP on counts for 5- and '
  write ( 3, '(a)' ) '4-letter cell counts.'

  write ( 3, 2345 ) filename
2345   format('   Test results for ',a15)

  do jk = 1, 25

    jkk = jkreset()
    s(0:624) = 0
    t(0:3124) = 0
!
!  Generate initial word with 5 random keystrokes.
!
    w = 625 * m8() + 125 * m8() + 25 * m8() + 5 * m8() + m8()

    do i1 = 1, 25600

      do i2 = 1, n
!
!  Erase leftmost letter of W.
!
        w = mod ( w, 5**4 )
!
!  Boost count for that 4-letter word.
!
        s(w) = s(w) + 1
!
!  Shift W left, add new letter, boost 5-letter word count.
!
        w = 5 * w + m8()
        t(w) = t(w) + 1

      end do

    end do
!
!  Find Q4: sum(obs-exp)**2/exp for 4-letter words.
!
    q4 = 0.0E+00

    do ii = 0, 5**4-1

      i = ii

      e = real ( 25600 * n )

      do j = 1, 4
        e = e * p( mod ( i, 5 ) ) * 2.0**(-8)
        i = i / 5
      end do

      q4 = q4 + ( s(ii) - e )**2 / e

    end do
!
!  Find Q5: sum(obs-exp)**2/exp for 5-letter words.
!
    q5 = 0.0E+00

    do ii = 0, 5**5-1
      i = ii
      e = real ( 25600 * n )
      do j = 1, 5
        e = e * p ( mod ( i, 5 ) ) * 2.0**(-8)
        i = i / 5
      end do
      q5 = q5 + ( t(ii) - e )**2 / e
    end do

    chsq = q5 - q4
    z = ( chsq - 2500.0 ) / sqrt ( 5000.0 )

    if ( jk == 1 ) then
      write(3,819) 25600*n
819       format(' Chi-square with 5^5-5^4 = 2500 d.of f. for sample size:', &
         i7,/,9x,'             chisquare  equiv normal  p value')
      write ( 3, '(a)' ) '  Results for COUNT-THE-1''s in specified bytes:'
    end if

    write(3,821) jk,jk+7,chsq,z,phi(z)
821     format('           bits ',i2,' to ',i2,f9.2,f11.3,f13.6)

    jkk = jkreset()

  end do

  return
end
subroutine craps_test ( filename )
!
!*******************************************************************************
!
!! CRAPS_TEST applies the craps test.
!
  implicit none
!
  real av
  real, parameter :: cc = 6.0 * 0.5**32
  real chisq
  real e(21)
  real ex
  character ( len = * ) filename
  integer i
  integer iwin
  integer j
  integer jkk
  integer jkreset
  integer jtbl
  integer k
  integer kthrow
  integer lp
  integer m
  integer, parameter :: ng = 200000
  integer nt(21)
  integer nthrows
  integer nwins
  real phi
  real pthrows
  real pwins
  real sd
  real t
  real total

  kthrow() = 2 + int ( cc * jtbl() + 3.0 ) + int ( cc * jtbl() + 3.0 )

  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'This is the CRAPS TEST. It plays 200,000 games '
  write ( *, '(a)' ) 'of craps, finds the number of wins and the number '
  write ( *, '(a)' ) 'of throws necessary to end each game.  '
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'The number of wins should be (very close to) a '
  write ( *, '(a)' ) 'normal with mean 200000p and variance 200000p(1-p), '
  write ( *, '(a)' ) 'with p=244/495.  '
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'Throws necessary to complete the game can vary'
  write ( *, '(a)' ) 'from 1 to infinity, but counts for all>21 are '
  write ( *, '(a)' ) 'lumped with 21.'
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'A chi-square test is made on the number-of-throws'
  write ( *, '(a)' ) 'cell counts.'
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'Each 32-bit integer from the test file provides '
  write ( *, '(a)' ) 'the value for the throw of a die, by floating '
  write ( *, '(a)' ) 'to [0,1), multiplying by 6 and taking 1 plus the '
  write ( *, '(a)' ) 'integer part of the result.'

  jkk = jkreset()

  e(1) = 1.0 / 3.0

  total = e(1)

  do k = 2, 20
    e(k) = ( 27.0 * ( 27.0 / 36.0 )**( k - 2 ) &
           + 40.0 * ( 26.0 / 36.0 )**( k - 2 ) &
           + 55.0 * ( 25.0 / 36.0 )**( k - 2 ) ) / 648.0
    total = total + e(k)
  end do

  e(21) = 1.0 - total

  nwins = 0

  nt(1:21) = 0

  do i = 1, ng

    lp = kthrow()
    nthrows = 1

    if ( lp == 7 .or. lp == 11 ) then

      iwin = 1

    else if ( lp == 2 .or. lp == 3 .or. lp == 12 ) then

      iwin = 0

    else

      do

        k = kthrow ( )
        nthrows = nthrows + 1

        if ( k == 7 ) then
          iwin = 0
          exit
        else if ( k == lp ) then
          iwin = 1
          exit
        end if

      end do

    end if

    m = min ( 21, nthrows )
    nt(m) = nt(m) + 1
    nwins = nwins + iwin

  end do

  av = 244.0 * ng / 495.0
  sd = sqrt ( av * 251.0 / 495.0 )
  t = ( nwins - av ) / sd

  write(3,2345) filename
2345  format(15x,' Results of craps test for ',a15)

  write ( 3, '(a)' ) '  No. of wins:  Observed Expected'
  write ( 3, '(15x,i12,f12.2)' ) nwins, av

   pwins = phi ( t )

   write(3,25) nwins,t,pwins
25     format(15x,i8,' =  No. of wins, z-score=',f6.3,' pvalue=',f7.5 )

  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) '  Analysis of Throws-per-Game:'

   total = 0.0
   do i = 1, 21
     ex = ng * e(i)
     total = total + ( nt(i) - ex )**2 / ex
   end do

   pthrows = chisq ( 20, total )

   write(3,24) total,pthrows
24     format(' Chisq = ',f7.2,' for 20 degrees of freedom, p=',f8.5 )

  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) '  Throws Observed Expected  Chisq     Sum'

  total = 0.0E+00
  do i = 1, 21
    ex = ng * e(i)
    total = total + ( nt(i) - ex )**2 / ex
    write(3,23) i,nt(i),ex,(nt(i)-ex)**2/ex, total
  end do

23     format(i19,i9,f11.1,f8.3,f9.3)
   write(3,2346) filename
2346   format('            SUMMARY  FOR ',a15)

   write ( 3, '(a,f8.6)' ) '  P-value for number of wins: ', pwins
   write ( 3, '(a,f8.6)' ) '  P-value for throws/game:    ', pthrows

   jkk = jkreset()

  return
end
subroutine d_swap ( x, y )
!
!*******************************************************************************
!
!! D_SWAP swaps two double precision values.
!
!
!  Modified:
!
!    01 May 2000
!
!  Author:
!
!    John Burkardt
!
!  Parameters:
!
!    Input/output, double precision X, Y.  On output, the values of X and
!    Y have been interchanged.
!
  implicit none
!
  double precision x
  double precision y
  double precision z
!
  z = x
  x = y
  y = z

  return
end
subroutine dvec_heap_d ( n, a )
!
!*******************************************************************************
!
!! DVEC_HEAP_D reorders a double precision array into an descending heap.
!
!
!  Definition:
!
!    A descending heap is an array A with the property that, for every index J,
!    A(J) >= A(2*J) and A(J) >= A(2*J+1), (as long as the indices
!    2*J and 2*J+1 are legal).
!
!  Diagram:
!
!                  A(1)
!                /      \
!            A(2)         A(3)
!          /     \        /  \
!      A(4)       A(5)  A(6) A(7)
!      /  \       /   \
!    A(8) A(9) A(10) A(11)
!
!  Modified:
!
!    07 July 2003
!
!  Author:
!
!    John Burkardt
!
!  Reference:
!
!    A Nijenhuis and H Wilf,
!    Combinatorial Algorithms,
!    Academic Press, 1978, second edition,
!    ISBN 0-12-519260-6.
!
!  Parameters:
!
!    Input, integer N, the size of the input array.
!
!    Input/output, double precision A(N).
!    On input, an unsorted array.
!    On output, the array has been reordered into a heap.
!
  implicit none
!
  integer n
!
  double precision a(n)
  integer i
  integer ifree
  double precision key
  integer m
!
!  Only nodes N/2 down to 1 can be "parent" nodes.
!
  do i = n/2, 1, -1
!
!  Copy the value out of the parent node.
!  Position IFREE is now "open".
!
    key = a(i)
    ifree = i

    do
!
!  Positions 2*IFREE and 2*IFREE + 1 are the descendants of position
!  IFREE.  (One or both may not exist because they exceed N.)
!
      m = 2 * ifree
!
!  Does the first position exist?
!
      if ( n < m ) then
        exit
      end if
!
!  Does the second position exist?
!
      if ( m + 1 <= n ) then
!
!  If both positions exist, take the larger of the two values,
!  and update M if necessary.
!
        if ( a(m) < a(m+1) ) then
          m = m + 1
        end if

      end if
!
!  If the large descendant is larger than KEY, move it up,
!  and update IFREE, the location of the free position, and
!  consider the descendants of THIS position.
!
      if ( a(m) <= key ) then
        exit
      end if

      a(ifree) = a(m)
      ifree = m

    end do
!
!  Once there is no more shifting to do, KEY moves into the free spot IFREE.
!
    a(ifree) = key

  end do

  return
end
subroutine dvec_sort_heap_a ( n, a )
!
!*******************************************************************************
!
!! DVEC_SORT_HEAP_A ascending sorts a double precision array using heap sort.
!
!
!  Modified:
!
!    07 July 2003
!
!  Author:
!
!    John Burkardt
!
!  Reference:
!
!    A Nijenhuis and H Wilf,
!    Combinatorial Algorithms,
!    Academic Press, 1978, second edition,
!    ISBN 0-12-519260-6.
!
!  Parameters:
!
!    Input, integer N, the number of entries in the array.
!
!    Input/output, double precision A(N).
!    On input, the array to be sorted;
!    On output, the array has been sorted.
!
  implicit none
!
  integer n
!
  double precision a(n)
  integer n1
!
  if ( n <= 1 ) then
    return
  end if
!
!  1: Put A into descending heap form.
!
  call dvec_heap_d ( n, a )
!
!  2: Sort A.
!
!  The largest object in the heap is in A(1).
!  Move it to position A(N).
!
  call d_swap ( a(1), a(n) )
!
!  Consider the diminished heap of size N1.
!
  do n1 = n-1, 2, -1
!
!  Restore the heap structure of A(1) through A(N1).
!
    call dvec_heap_d ( n1, a )
!
!  Take the largest object from A(1) and move it to A(N1).
!
    call d_swap ( a(1), a(n1) )

  end do

  return
end
subroutine i_swap ( i, j )
!
!*******************************************************************************
!
!! I_SWAP swaps two integer values.
!
!
!  Modified:
!
!    30 November 1998
!
!  Author:
!
!    John Burkardt
!
!  Parameters:
!
!    Input/output, integer I, J.  On output, the values of I and
!    J have been interchanged.
!
  integer i
  integer j
  integer k
!
  k = i
  i = j
  j = k

  return
end
subroutine ivec_heap_d ( n, a )
!
!*******************************************************************************
!
!! IVEC_HEAP_D reorders an array of integers into an descending heap.
!
!
!  Definition:
!
!    A descending heap is an array A with the property that, for every index J,
!    A(J) >= A(2*J) and A(J) >= A(2*J+1), (as long as the indices
!    2*J and 2*J+1 are legal).
!
!  Diagram:
!
!                  A(1)
!                /      \
!            A(2)         A(3)
!          /     \        /  \
!      A(4)       A(5)  A(6) A(7)
!      /  \       /   \
!    A(8) A(9) A(10) A(11)
!
!  Modified:
!
!    15 April 1999
!
!  Author:
!
!    John Burkardt
!
!  Reference:
!
!    A Nijenhuis and H Wilf,
!    Combinatorial Algorithms,
!    Academic Press, 1978, second edition,
!    ISBN 0-12-519260-6.
!
!  Parameters:
!
!    Input, integer N, the size of the input array.
!
!    Input/output, integer A(N).
!    On input, an unsorted array.
!    On output, the array has been reordered into a heap.
!
  implicit none
!
  integer n
!
  integer a(n)
  integer i
  integer ifree
  integer key
  integer m
!
!  Only nodes N/2 down to 1 can be "parent" nodes.
!
  do i = n/2, 1, -1
!
!  Copy the value out of the parent node.
!  Position IFREE is now "open".
!
    key = a(i)
    ifree = i

    do
!
!  Positions 2*IFREE and 2*IFREE + 1 are the descendants of position
!  IFREE.  (One or both may not exist because they exceed N.)
!
      m = 2 * ifree
!
!  Does the first position exist?
!
      if ( n < m ) then
        exit
      end if
!
!  Does the second position exist?
!
      if ( m + 1 <= n ) then
!
!  If both positions exist, take the larger of the two values,
!  and update M if necessary.
!
        if ( a(m) < a(m+1) ) then
          m = m + 1
        end if

      end if
!
!  If the large descendant is larger than KEY, move it up,
!  and update IFREE, the location of the free position, and
!  consider the descendants of THIS position.
!
      if ( a(m) <= key ) then
        exit
      end if

      a(ifree) = a(m)
      ifree = m

    end do
!
!  Once there is no more shifting to do, KEY moves into the free spot IFREE.
!
    a(ifree) = key

  end do

  return
end
subroutine ivec_sort_heap_a ( n, a )
!
!*******************************************************************************
!
!! IVEC_SORT_HEAP_A ascending sorts an integer array using heap sort.
!
!
!  Modified:
!
!    15 April 1999
!
!  Author:
!
!    John Burkardt
!
!  Reference:
!
!    A Nijenhuis and H Wilf,
!    Combinatorial Algorithms,
!    Academic Press, 1978, second edition,
!    ISBN 0-12-519260-6.
!
!  Parameters:
!
!    Input, integer N, the number of entries in the array.
!
!    Input/output, integer A(N).
!    On input, the array to be sorted;
!    On output, the array has been sorted.
!
  implicit none
!
  integer n
!
  integer a(n)
  integer n1
!
  if ( n <= 1 ) then
    return
  end if
!
!  1: Put A into descending heap form.
!
  call ivec_heap_d ( n, a )
!
!  2: Sort A.
!
!  The largest object in the heap is in A(1).
!  Move it to position A(N).
!
  call i_swap ( a(1), a(n) )
!
!  Consider the diminished heap of size N1.
!
  do n1 = n-1, 2, -1
!
!  Restore the heap structure of A(1) through A(N1).
!
    call ivec_heap_d ( n1, a )
!
!  Take the largest object from A(1) and move it to A(N1).
!
    call i_swap ( a(1), a(n1) )

  end do

  return
end
function jtbl8 ( )
!
!*******************************************************************************
!
!! JTBL8 returns the next byte from the file of random 32 bit integers.
!
  implicit none
!
  integer b(4096)
  integer, save :: j = 4097
  integer, save :: jk = 1
  integer jtbl8
  integer k
  integer, save :: nleft = 0
!
  if ( 4096 < j ) then
    read ( 1, rec = jk ) b
    j = 1
    jk = jk + 1
  end if

  if ( nleft == 0 ) then
    k = b(j)
    j = j + 1
    nleft = 4
  end if

  jtbl8 = and ( rshift ( k, 24 ), 255 )
  k = lshift ( k, 8 )
  nleft = nleft - 1

  return
end
function jtbl ( )
!
!*******************************************************************************
!
!! JTBL returns the next integer from the file of random 32 bit integers.
!
  implicit none
!
  integer b(4096)
  integer, save :: j = 4097
  integer, save :: jk = 1
  integer jkreset
  integer jtbl

  if ( 4096 < j ) then
    read ( 1, rec = jk ) b
    j = 1
    jk = jk + 1
  end if

  jtbl = b(j)
  j = j + 1

  return

  entry jkreset()
  jk = 1
  jkreset = 1
  return
end
function kp ( c )
!
!*******************************************************************************
!
!! KP ???
!
  implicit none
!
  integer b(5)
  integer c(5)
  integer i
  integer j
  integer k
  integer kp
  integer, dimension ( 0:59) :: map = (/ &
    39, &
    38, 37, 36, 41, 40, 54, 55, 56, 57, 58, &
    59, 49, 48, 52, 53, 50, 51, 42, 43, 44, &
    45, 46, 47, 33, 32, 31, 30, 35, 34, 12, &
    13, 14, 15, 16, 17, 29, 28, 24, 25, 27, &
    26, 21, 20, 19, 18, 23, 22,  2,  3,  5, &
     4,  1,  0, 10, 11,  9,  8,  6,  7 /)
  integer t

  b(1:5) = c(1:5)
  kp = 0

  do i = 5, 2, -1

    t = b(1)
    k = 1

    do j = 2, i
      if ( t <= b(j) ) then
        t = b(j)
        k = j
      end if
    end do

    kp = i * kp + k - 1

    call i_swap ( b(i), b(k) )

  end do

  if ( kp < 60 ) then
    kp = map(kp)
  end if

  return
end
subroutine ks_test ( n, y, p )
!
!*******************************************************************************
!
!! KS_TEST applies the Kolmorogov-Smirnov test.
!
!
!  Discussion:
!
!    The test is based on the distance between the empirical and theoretical
!    distribution functions.
!
!  Parameters:
!
!    Input, integer N, the number of elements of Y.
!
!    Input, real Y(N), an array of numbers, supposed to be drawn
!    from a uniform distribution in [0,1].
!
!    Output, real P, the probability associated with the observed value
!    of the Anderson-Darling statistic, which is N times the integral
!    of ( FN(X) - X )**2 / ( X * ( 1 - X ) )
!
  implicit none

  integer n
!
  integer i
  real p
  real t
  real y(n)
  real z

  call rvec_sort_heap_a ( n, y )

  z = - real ( n ) * real ( n )

  do i = 1, n

    t = y(i) * ( 1.0 - y(n+1-i) )

    if ( t < 1.0E-20 ) then
      t = 1.0E-20
    end if

    z = z - ( i + i - 1 ) * log ( t )

  end do

  z = z / real ( n )

  if ( z < 0.01 ) then

    p = 0.0

  else if ( z <= 2.0 ) then

    p = 2.0 * exp ( -1.2337 / z ) &
      * ( 1.0 + z / 8.0 - 0.04958 * z * z / ( 1.325 + z ) ) / sqrt ( z )

  else if ( 4.0 < z ) then

    p = 1.0 - 0.4938691 * exp ( -1.050321 * z ) &
      - 0.5946335 * exp ( -1.527198 * z )

  else

    p = 1.0 - 0.6621361 * exp ( -1.091638 * z ) &
      - 0.95059 * exp ( -2.005138 * z )

  end if

  return
end
subroutine min_dist_test ( filename )
!
!*******************************************************************************
!
!! MIN_DIST_TEST applies the minimum distance test.
!
!
!  Discussion:
!
!    Compute the minimum distance^2 between N random points ( X(I), Y(I) ).
!
!    The mean is about 0.64 for 4000 points in a square of side 1000,
!    and 0.995 for 8000 points in a square of side 10000.
!
!    Since distance^2 is approximately exponential with mean 0.04,
!    the quantity 1.0-exp(-d^2/0.04) should be uniform on [0,1).  
!    Thus a KS test is applied to the results.
!
  implicit none
!
  real d
  real dmin
  character ( len = * ) filename
  real g(100)
  integer i
  integer ij
  integer j
  integer jkk
  integer jkreset
  integer jtbl
  integer, parameter :: n = 8000
  integer, parameter :: ns = 100
  real p
  real*8 qq(8000)
  real total
  real u
  real uni
  real v
  real xy(16000)

  character*80 text(13), dum
  equivalence(qq(1),xy(1))
!
!  one line function to generate a random coordinate in [0,1000).
!
  uni() = 5000.0 + jtbl() * 0.2328306E-05

  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'THE MINIMUM DISTANCE TEST '
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'It does this 100 times:: choose n = 8000 random '
  write ( 3, '(a)' ) 'points in a square of side 10000.  Find D, the '
  write ( 3, '(a)' ) 'minimum distance between the (n^2-n)/2 pairs of '
  write ( 3, '(a)' ) 'points.  If the points are truly independent '
  write ( 3, '(a)' ) 'uniform, then d^2, the square of the minimum '
  write ( 3, '(a)' ) 'distance should be (very close to) exponentially '
  write ( 3, '(a)' ) 'distributed with mean 0.995 .  Thus, the'
  write ( 3, '(a)' ) 'quantity 1-exp(-d^2/0.995) should be uniform '
  write ( 3, '(a)' ) 'on [0,1) and a KS test on the resulting 100 '
  write ( 3, '(a)' ) 'values serves as a test of uniformity for random '
  write ( 3, '(a)' ) 'points in the square.  Test numbers = 0 mod 5'
  write ( 3, '(a)' ) 'are printed but the KS test is based on the full '
  write ( 3, '(a)' ) 'set of 100 random choices of 8000 points in the '
  write ( 3, '(a)' ) '10000 x 10000 square.'

  jkk = jkreset()

  write(3,441) filename
441   format('               This is the MINIMUM DISTANCE test',/, &
     '              for random integers in the file ',a15,/, &
     4x,' Sample no.    d^2     avg     equiv uni            ')

  total = 0.0

  do ij = 1, ns

    dmin = huge ( dmin )

    do i = 1, 16000
      xy(i) = uni()
    end do

    call dvec_sort_heap_a ( 8000, qq )

    do i = 2, 16000, 2

      u = xy(i)
      v = xy(i-1)

      do j = i + 2, 16000, 2

        d = ( u - xy(j) )**2

        if ( dmin <=  d ) then
          exit
        end if

        d = d + ( v - xy(j-1) )**2

        dmin = min ( dmin, d )

      end do

    end do

    d = dmin
    total = total + d
    g(ij) = 1.0 - exp ( -dmin / 0.995 )

    if ( mod ( ij, 5 ) == 0 ) then
      write(3,23) ij,d,total/ij,g(ij)
    end if
23      format(i12,f10.4,f9.4,f12.6)

  end do

  write(3,35) filename
35       format('     MINIMUM DISTANCE TEST for ',a15)
  write(3,445)
445   format(10x,'Result of KS test on 20 transformed mindist^2''s:')

  call ks_test ( ns, g, p )

  write ( 3, '(a,f8.6)' ) '  P-value from KS test for 100 tests = ', p

  jkk = jkreset()

  return
end
subroutine omso ( filename )
!
!*******************************************************************************
!
!! OMSO ???
!
!
!  Discussion:
!
!    NUMBER OF MISSING WORDS IN A STRING OF 2**21 k-LETTER WORDS,
!    EACH LETTER 20/k BITS. THERE ARE 2**20 POSSIBLE WORDS.
!
!    Each of the 32 bits in the 2**15 W-TABLE IDENTIFIES a word.
!
!    The mean should be 141,909 with sigma = 290.
!
  implicit none
!
  character ( len = * ) filename
  integer i
  integer ic
  integer ikbit
  integer j
  integer jk
  integer jkk
  integer jkreset
  integer jtbl
  integer k
  integer kij
  integer kk
  integer kount
  integer, parameter :: kpow = 21
  integer kr
  integer krk
  integer l
  integer lk
  integer mbit(0:31)
  integer mk
  integer mkk
  integer nt
  integer, parameter :: ntries = 1
  real phi
  real, dimension ( 3 ) :: sigs = (/ 290.0E+00, 295.0E+00, 339.0E+00 /)
  real true
  integer w(0:32767)
  real x

   character*4 ctest(3)
   data ctest/'OPSO','OQSO',' DNA'/
!
!  ONE-LINE FUNCTION TO GENERATE 5-BIT LETTER IN CONVENIENT POSITION
!
  ikbit() = and ( rshift ( jtbl(), kr ), mk )

  j = 1232456789

  do i = 1, 1000000
    j = 69069 * j
  end do

  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'The tests OPSO, OQSO and DNA'
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'OPSO means Overlapping-Pairs-Sparse-Occupancy'
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'The OPSO test considers 2-letter words from an '
  write ( 3, '(a)' ) 'alphabet of 1024 letters.  Each letter is '
  write ( 3, '(a)' ) 'determined by a specified ten bits from a 32-bit '
  write ( 3, '(a)' ) 'integer in the sequence to be tested.  OPSO'
  write ( 3, '(a)' ) 'generates  2^21 (overlapping) 2-letter words  '
  write ( 3, '(a)' ) '(from 2^21+1 "keystrokes")  and counts the '
  write ( 3, '(a)' ) 'number of missing words---that is 2-letter words'
  write ( 3, '(a)' ) 'which do not appear in the entire sequence. '
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'That count should be very close to normally '
  write ( 3, '(a)' ) 'distributed with mean 141,909, sigma 290. Thus '
  write ( 3, '(a)' ) '(missingwrds-141909)/290 should be a standard '
  write ( 3, '(a)' ) 'normal variable. The OPSO test takes 32 bits at'
  write ( 3, '(a)' ) 'a time from the test file and uses a designated '
  write ( 3, '(a)' ) 'set of ten consecutive bits. It then restarts '
  write ( 3, '(a)' ) 'the file for the next designated 10 bits, and so on.'
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'OQSO means Overlapping-Quadruples-Sparse-Occupancy'
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'The test OQSO is similar, except that it considers '
  write ( 3, '(a)' ) '4-letter words from an alphabet of 32 letters, '
  write ( 3, '(a)' ) 'each letter determined by a designated string '
  write ( 3, '(a)' ) 'of 5 consecutive bits from the test file, elements '
  write ( 3, '(a)' ) 'of which are assumed 32-bit random integers.'
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'The mean number of missing words in a sequence of '
  write ( 3, '(a)' ) '2^21 four-letter words,  (2^21+3 "keystrokes"), '
  write ( 3, '(a)' ) 'is again 141909, with sigma = 295.  The mean is '
  write ( 3, '(a)' ) 'based on theory; sigma comes from extensive '
  write ( 3, '(a)' ) 'simulation.'
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'The DNA test considers an alphabet of 4 letters:'
  write ( 3, '(a)' ) 'C,G,A,T,, determined by two designated bits in '
  write ( 3, '(a)' ) 'the sequence of random integers being tested.  '
  write ( 3, '(a)' ) 'It considers 10-letter words, so that as in OPSO '
  write ( 3, '(a)' ) 'and OQSO, there are 2^20 possible words, and the'
  write ( 3, '(a)' ) 'mean number of missing words from a string of 2^21  '
  write ( 3, '(a)' ) '(overlapping) 10-letter words (2^21+9 "keystrokes")'
  write ( 3, '(a)' ) 'is 141909.'
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'The standard deviation sigma=339 was determined '
  write ( 3, '(a)' ) 'as for OQSO by simulation.  (Sigma for OPSO, '
  write ( 3, '(a)' ) '290, is the true value (to three places), not '
  write ( 3, '(a)' ) 'determined by simulation.'

  do jk = 1, 3

    k = 2 * jk
    if ( jk == 3 ) then
      k = 10
    end if

    lk = 20 / k

    write ( *, '(a)' ) ' Output: No. missing words (mw), ' // &
      'equiv normal variate (z), p-value (p)'
    write ( *, '(a)' ) 'mw     z     p'
    write(3,*) ctest(jk),' test for generator ',filename
    write(3,*)' Output: No. missing words (mw), ' // &
      'equiv normal variate (z), p-value (p)'
    write(3,*)'   mw     z     p'

    do krk = 33 - lk, 1, -1

      jkk = jkreset()
      kr = 33 - lk - krk
      mk = 2**( lk ) - 1
      mkk = 2**( 20 - lk ) - 1

      do kij = 1, ntries
!
!  Set mask bits.
!
        mbit(0) = 1
        do i = 1, 31
          mbit(i) = 2 * mbit(i-1)
        end do
!
!  Initialize.
!
        true = real ( 2**20 ) * exp ( - 2.0**( kpow - 20 ) )

        do nt = 1, ntries

          w(0:32767) = 0
!
!  Get initial word.
!
          j = ikbit()
          do i = 1, k-1
            j = 2**( lk ) * j + ikbit()
          end do
!
!  Generate 2**KPOW overlapping words.
!
          do ic = 1, 2**kpow
!
!  Get new J.
!
            j = lshift ( and ( j, mkk ), lk ) + ikbit ( )
!
!  Get bit index from last 5 bits of J.
!
            l = and ( j, 31 )
!
!  Get table index from leading 15 bits of J.
!
            kk = rshift ( j, 5 )
!
!  Set bit L in W(KK).
!
            w(kk) = or ( w(kk), mbit(l) )

          end do
!
!  Count number of empty cells.
!
          kount = 0
          do kk = 0, 32767
            do l = 0, 31
              if ( and ( w(kk), mbit(l) ) == 0 ) then
                kount = kount + 1
              end if
            end do
          end do

          x = ( kount - true ) / sigs(jk)
          write(3,22) ctest(jk),filename,33-20/k-kr,32-kr,KOUNT,x, phi(x)

        end do

22          format(a8,' for ',a15,' using bits ',i2, &
            ' to ',i2,i14,f7.3,f7.4)
        jkk = jkreset()

      end do

    end do

  end do

  return
end
subroutine overlap_perm_test ( filename )
!
!*******************************************************************************
!
!! OVERLAP_PERM_TEST applies the overlapping 5-permutations test.
!
!
!  Discussion:
!
!    The overlapping 5-permutations test uses the 120 x 120 weak inverse
!    of the covariance matrix, in 60 x 60 blocks.
!
!    69069 passes, 
!    Randu fails, 
!    Weyl fails, 
!    SR(15,17) fails
!    SR(13,18) fails.
!    F(2,1,*) passes
!    F(3,1,*) passes
!
  implicit none
!
  real av
  real chisq
  real chsq
  character ( len = * ) filename
  integer i
  integer ijkm
  integer ios
  integer j
  integer jkk
  integer jkreset
  integer jtbl
  integer k
  integer kp
  integer n
  integer r(60,60)
  integer s(60,60)
  integer t(120)
  integer u(1005)
  real x
  real y

  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'THE OVERLAPPING 5-PERMUTATION TEST'
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'This test looks at a sequence of one million 32-bit '
  write ( *, '(a)' ) 'random integers.  Each set of five consecutive'
  write ( *, '(a)' ) 'integers can be in one of 120 states, for the '
  write ( *, '(a)' ) '5! possible orderings of five numbers.  Thus '
  write ( *, '(a)' ) 'the 5th, 6th, 7th,...numbers each provide a '
  write ( *, '(a)' ) 'state. '
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'As many thousands of state transitions are '
  write ( *, '(a)' ) 'observed, cumulative counts are made of the '
  write ( *, '(a)' ) 'number of occurences of each state.  Then the '
  write ( *, '(a)' ) 'quadratic form in the weak inverse of the '
  write ( *, '(a)' ) '120x120 covariance matrix yields a test equivalent '
  write ( *, '(a)' ) 'to the likelihood ratio test that the 120 cell'
  write ( *, '(a)' ) 'counts came from the specified (asymptotically) '
  write ( *, '(a)' ) 'normal distribution with the specified 120x120 '
  write ( *, '(a)' ) 'covariance matrix (with rank 99).  This version '
  write ( *, '(a)' ) 'uses 1,000,000 integers, twice.'
!
!  Divide r and s elements by (200000*n) for proper cov. inverse.
!
!  The rank is 99 = 50 + 49.
!
  open ( 4, file = 'perm.txt', iostat = ios )

  if ( ios /= 0 ) then
    write ( *, '(a)' ) ' '
    write ( *, '(a)' ) 'OVERLAP_PERM_TEST - Fatal error!'
    write ( *, '(a)' ) '  Could not open the file PERM.TXT.'
    return
  end if

  read ( 4, '(8i10)' ) ((r(i,j),j = i,60),i=1,60)

  do i = 1, 59
    do j = i + 1, 60
      r(j,i) = r(i,j)
    end do
  end do

  read ( 4, '(8i10)' ) ((s(i,j),j = i,60),i=1,60)

  do i = 1, 59
    do j = i + 1, 60
      s(j,i) = s(i,j)
    end do
  end do

  close ( unit = 4 )

  do ijkm = 1, 2
!
!  Get counts t(1),...,t(120).
!
    jkk = jkreset()
    n = 1000
    t(1:120) = 0

    do i = 1001, 1005
      u(i) = jtbl()
    end do

    do i = 1, n

      do j = 1, 5
        u(j) = u(1000+j)
      end do

      do j = 1, 1000
        k = kp ( u(j) ) + 1
        t(k) = t(k) + 1
        u(j+5) = jtbl( )
      end do

    end do
!
!  Evalute quadratic form in weak inverse.
!
    chsq = 0.0E+00
    av = real ( n ) * 2000.0E+00 / 120.0E+00
    do i = 1, 60
      x = t(i) + t(i+60) - av
      y = t(i) - t(i+60)
      do j = 1, 60
        chsq = chsq + x * r(i,j) * ( t(j) + t(j+60) - av ) &
                    + y * s(i,j) * ( t(j) - t(j+60) )
      end do
    end do

    chsq = chsq / ( 2.0E+08 * real ( n ) )

    write(3,271) filename
271     format('           OPERM5 test for file ',a15,/, &
       '     For a sample of 1,000,000 consecutive 5-tuples,')
    write(3,273) chsq, chisq ( 99, chsq )
273     format(' chisquare for 99 degrees of freedom = ',f7.3,'; p-value=',f8.6)

  end do

  jkk = jkreset()

  return
end
subroutine overlap_sum_test ( filename )
!
!*******************************************************************************
!
!! OVERLAP_SUM_TEST performs the overlapping sums test.
!
  implicit none
!
  integer, parameter :: m = 100
!
  real a
  real b
  real, dimension (0:100) :: f = (/ &
    0.0000, &
    0.0017, 0.0132, 0.0270, 0.0406, 0.0538, &
    0.0665, 0.0787, 0.0905, 0.1020, 0.1133, &
    0.1242, 0.1349, 0.1454, 0.1557, 0.1659, &
    0.1760, 0.1859, 0.1957, 0.2054, 0.2150, &
    0.2246, 0.2341, 0.2436, 0.2530, 0.2623, &
    0.2716, 0.2809, 0.2902, 0.2995, 0.3087, &
    0.3180, 0.3273, 0.3366, 0.3459, 0.3552, &
    0.3645, 0.3739, 0.3833, 0.3928, 0.4023, &
    0.4118, 0.4213, 0.4309, 0.4406, 0.4504, &
    0.4602, 0.4701, 0.4800, 0.4900, 0.5000, &
    0.5100, 0.5199, 0.5299, 0.5397, 0.5495, &
    0.5593, 0.5690, 0.5787, 0.5882, 0.5978, &
    0.6073, 0.6167, 0.6260, 0.6354, 0.6447, &
    0.6540, 0.6632, 0.6724, 0.6817, 0.6910, &
    0.7003, 0.7096, 0.7189, 0.7282, 0.7375, &
    0.7468, 0.7562, 0.7657, 0.7752, 0.7848, &
    0.7944, 0.8041, 0.8140, 0.8239, 0.8340, &
    0.8442, 0.8545, 0.8650, 0.8757, 0.8867, &
    0.8980, 0.9095, 0.9214, 0.9337, 0.9464, &
    0.9595, 0.9731, 0.9868, 0.9983, 1.0000 /)
  character*15 filename
  real h
  integer i
  integer ij
  integer ik
  integer j
  integer jkk
  integer jkreset
  integer jtbl
  real p
  real phi
  real pk
  real qq
  real s
  real t(199)
  real u(m)
  real uni
  real w(10)
  real x(m)
  real y(m)

  uni() = jtbl() * 0.806549E-09

  jkk = jkreset()

  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'The OVERLAPPING SUMS test'
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'Integers are floated to get a sequence U(1),'
  write ( *, '(a)' ) 'U(2),... of uniform [0,1) variables.  Then '
  write ( *, '(a)' ) 'overlapping sums,'
  write ( *, '(a)' ) '  S(1) = U(1) +...+ U(100),'
  write ( *, '(a)' ) '  S(2) = U(2) +...+ U(101),'
  write ( *, '(a)' ) 'are formed.'
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'The S''s are virtually normal with a certain'
  write ( *, '(a)' ) 'covariance matrix.  A linear transformation of '
  write ( *, '(a)' ) 'the S''s converts them to a sequence of '
  write ( *, '(a)' ) 'independent standard normals, which are converted'
  write ( *, '(a)' ) 'to uniform variables for a KS test. '
  write ( *, '(a)' ) 'The P-values from ten KS tests are given still '
  write ( *, '(a)' ) 'another KS test.'

  do ik = 1, 10

    do ij = 1, m

      s = 0.0E+00
      do i = 1, 199
        t(i) = uni()
        if ( i <= m ) then
          s = s + t(i)
        end if
      end do

      y(1) = s
      do j = 2, m
        y(j) = y(j-1) - t(j-1) + t(m+j-1)
      end do
!
!  Now y(j) = z(j)+...+z(99+j)
!  They are virtually normal, mean 0, variance 100, but correlated.
!  Now a matrix transformation of the y's: x = yM, will make the
!  x's independent normal.
!
!  The y's covariance matrix T is Toeplitz with diagonals 100,99,...2,1
!  A Cholesky factorization of T: V'V = T provides M=V^(-1).  The
!  covariance of x = yM is M'TM=I.
!
!  The columns of M have at most 3 non-zero elements.
!
      x(1) = y(1) / sqrt ( real ( m ) )
      x(2) = - real ( m - 1 ) * y(1) / sqrt ( m * ( m + m - 1. ) ) &
        + y(2) * sqrt ( m / ( m + m - 1. ) )
      qq = x(1)**2 + x(2)**2

      do i = 3, m
        a = real ( m + m + 2 - i )
        b = real ( 4 * m + 2 - i - i )
        x(i) = y(1) / sqrt ( a * b ) &
          - sqrt ( ( a - 1.0 ) / ( b + 2.0 ) ) * y(i-1) + sqrt ( a / b ) * y(i)
        qq = qq + x(i)**2
      end do
!
!  Now the x's are a bunch of iid rnors with mean 0, variance 1.
!  Does sum(x(i)^2) behave as chisquare with M degrees of freedom?
!
!  Convert  x(1),...,x(m) to uni's.
!
      do i = 1, m
        p = phi ( x(i) )
        h = 100.0 * p
        j = h
        x(i) = f(j) + ( h - j ) * ( f(j+1) - f(j) )
      end do
!
!  Test to see if the transformed x's are uniforms.
!
      call ks_test ( m, x, p )

      u(ij) = p

    end do
!
!  Now do a KS test on the 100 p's from the tests for normality.
!
    call ks_test ( 100, u, pk )

    w(ik) = pk
    write ( 3, '(a,i2,a,f8.6)' ) '  Test # ', ik, '  P-value = ', pk

  end do

  write(3,2345) filename
2345  format('   Results of the OVERLAP_SUM test for ',a15)

  call ks_test ( 10, w, p )

  write ( 3, '(a,f8.6)' ) '  P-value from KS test for 10 tests = ', p

  jkk = jkreset()

  return
end
subroutine parking_test ( filename )
!
!*******************************************************************************
!
!! PARKING_TEST applies the parking lot test.
!
  implicit none
!
  logical add
  real av
  character ( len = * ) filename
  real g(10)
  integer i
  integer ij
  integer j
  integer jkk
  integer jkreset
  integer jtbl
  integer k
  integer n
  integer, parameter :: nt = 10
  integer, parameter :: ntries = 12000
  real phi
  real pp
  real s
  real sig
  real, parameter :: sq = 100.0
  real ss
  real uni
  real w
  real x(4000)
  real y(4000)
  real z

  character*80 text(22), dum

  uni() = 0.5 + 0.5**32 * jtbl()

  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'THE PARKING LOT TEST'
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'In a square of side 100, randomly "park" a car,'
  write ( 3, '(a)' ) 'a circle of radius 1.   Then try to park a 2nd, '
  write ( 3, '(a)' ) 'a 3rd, and so on, each time parking "by ear".  '
  write ( 3, '(a)' ) 'That is, if an attempt to park a car causes a '
  write ( 3, '(a)' ) 'crash with one already parked, try again at a '
  write ( 3, '(a)' ) 'new random location. (To avoid path problems, '
  write ( 3, '(a)' ) 'consider parking helicopters rather than cars.)'
  write ( 3, '(a)' ) 'Each attempt leads to either a crash or a '
  write ( 3, '(a)' ) 'success, the latter followed by an increment to'
  write ( 3, '(a)' ) 'the list of cars already parked. '
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'If we plot N, the number of attempts, versus K,'
  write ( 3, '(a)' ) 'the number successfully parked, we get a curve '
  write ( 3, '(a)' ) 'that should be similar to those provided by a '
  write ( 3, '(a)' ) 'perfect random number generator.  '
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'Theory for the behavior of such a random curve '
  write ( 3, '(a)' ) 'seems beyond reach, and as graphics displays are'
  write ( 3, '(a)' ) 'not available for this battery of tests, a simple '
  write ( 3, '(a)' ) 'characterization of the random experiment is used: '
  write ( 3, '(a)' ) 'K, the number of cars successfully parked after '
  write ( 3, '(a)' ) 'N = 12,000 attempts. '
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'Simulation shows that K should average 3523 with '
  write ( 3, '(a)' ) 'sigma 21.9 and is very close to normally '
  write ( 3, '(a)' ) 'distributed.  Thus (K-3523)/21.9 should be a '
  write ( 3, '(a)' ) 'standard normal variable, which, converted to '
  write ( 3, '(a)' ) 'a uniform variable, provides input to a KS test '
  write ( 3, '(a)' ) 'based on a sample of 10.'

  write(3,2345) filename
2345  format(10x,' PARK: result of ten tests on file ',a15)

  jkk = jkreset()

  s = 0.0
  ss = 0.0

  do ij = 1, nt

    x(1) = sq * uni()
    y(1) = sq * uni()
    k = 1

    do n = 1, ntries

      z = sq * uni()
      w = sq * uni()

      add = .true.

      do i = 1, k
        if ( abs ( x(i) - z ) <= 1.0 .and. abs ( y(i) - w ) <= 1.0 ) then
          add = .false.
          exit
        end if
      end do

      if ( add ) then
        k = k + 1
        x(k) = z
        y(k) = w
      end if

    end do

    s = s + k
    ss = ss + k * k
    z = ( real ( k ) - 3523.0 ) / 21.9
    g(ij) = phi ( z )

    write(3,27) k,z,g(ij)
27      format(10x,'  Successes:',i5,'    z-score:',f7.3,' p-value:',f8.6)

  end do

  av = s / real ( nt )
  sig = ss / real ( nt ) - av**2

  write ( 3, '(a)' ) ' '
  write(3,25) sq,av,sqrt(sig)
25    format(10x,' square size   avg. no.  parked   sample sigma',/, &
     10x,f7.0,f20.3,f13.3)

  call ks_test ( 10, g, pp )

  write ( 3, '(a,f8.6)' ) '  P-value from KS test for 10 tests = ', pp

  jkk = jkreset()

  return
end
function phi ( x )
!
!*******************************************************************************
!
!! PHI ???
!
!
!  Discussion:
!
!    I suspect PHI is an inverse normal distribution function...
!
  implicit none
!
  double precision a
  double precision b
  real*8 cphi
  double precision h
  integer i
  real*8 j
  real phi
  double precision pwr
  double precision total
  real*8, dimension(0:14) :: v = (/ &
    1.253314137315500D+00,  0.6556795424187985D+00, 0.4213692292880545D+00,  &
    0.3045902987101033D+00, 0.2366523829135607D+00, 0.1928081047153158D+00,  &
    0.1623776608968675D+00, 0.1401041834530502D+00, 0.1231319632579329D+00,  &
    0.1097872825783083D+00, 0.9902859647173193D-01, 0.9017567550106468D-01, &
    0.8276628650136917D-01, 0.764757610162485D-01,  0.7106958053885211D-01 /)
  real x
  double precision z

  phi = 0.5E+00 + sign ( 0.5E+00, x )

  if ( 7.0E+00 < abs ( x ) ) then
    return
  end if

  cphi = 0.5D+00 - sign ( 0.5D+00, dble ( x ) )
  j = abs ( dble ( x ) ) + 0.5D+00
  j = min ( j, 14.0D+00 )
  z = j
  h = abs ( dble ( x ) ) - z
  a = v(j)
  b = z * a - 1.0D+00
  pwr = 1.0D+00
  total = a + h * b

  do i = 2, 24-j, 2
    a = ( a + z * b ) / dble ( i )
    b = ( b + z * a ) / dble ( i + 1 )
    pwr = pwr * h**2
    total = total + pwr * ( a + h * b )
  end do

  cphi = total * exp ( - 0.5D+00 * dble ( x ) * dble ( x ) &
    - 0.918938533204672D+00 )

  if ( x < 0.0E+00 ) then
    phi = real ( cphi )
  else
    phi = real ( 1.0D+00 - cphi )
  end if

  return
end
subroutine rank31_test ( filename )
!
!*******************************************************************************
!
!! RANK31_TEST performs the rank 31 test.
!
!
!  Discussion:
!
!    see original file \f\bprint.for that displays each step in the
!    rank reduction.
!    finds rank of 31x31 matrices.
!    uses 31 leftmost bits of a 32-bit integer
!    to form a row of the binary matrix.
!
!      function mrank(r,m,n)
!
!    for nxn matrices, to at least 6 places,
!    the probability of rank n-2,n-1,n are all virtually the same.
!    r          p
!    < = 29  .0052854502
!    30      .1283502644
!    31      .5775761902
!    32      .2887880952
!
!    Finds binary rank of m rows, n trailing bits each.
!
  implicit none
!
  integer, parameter :: m = 31
!
  real chisq
  real d
  real e
  character ( len = * ) filename
  integer i
  integer ij
  integer j
  integer jkk
  integer jkreset
  integer jtbl
  integer k
  integer n
  integer, parameter :: ntries = 40000
  integer ntry
  real, dimension (0:3) :: p = (/ &
    0.2887880952E+00, 0.5775761902E+00, 0.1283502644E+00, 0.0052854502E+00 /)
  integer rank
  integer row(m)
  real s
  integer tbl(0:3)

  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'This is the BINARY RANK TEST for 31x31 matrices. '
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'The leftmost 31 bits of 31 random integers from '
  write ( 3, '(a)' ) 'the test sequence are used to form a 31x31 '
  write ( 3, '(a)' ) 'binary matrix over the field {0,1}. The rank'
  write ( 3, '(a)' ) 'is determined. That rank can be from 0 to 31, '
  write ( 3, '(a)' ) 'but ranks< 28 are rare, and their counts are '
  write ( 3, '(a)' ) 'pooled with those for rank 28.'
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'Ranks are found for 40,000 such random matrices '
  write ( 3, '(a)' ) 'and a chisquare test is performed on counts for '
  write ( 3, '(a)' ) 'ranks 31,30,29 and <=28.'

  jkk = jkreset()

  n = m

  write(3,2345) filename
2345    format('    Binary rank test for ',a15)
  write(3,29) m,n
29      format(7x,'  Rank test for ',i2,'x',i2,' binary matrices:')
  write(3,291) m
291     format(7x,' rows from leftmost ',i2, &
       ' bits of each 32-bit integer')

  tbl(0:3) = 0

  do ij = 1, ntries

    do i = 1, m
      row(i) = rshift ( jtbl(), 32 - m )
    end do

    ntry = ntry + 1

    k = rank ( m, n, row )
    k = min ( n - k, 3 )
    tbl(k) = tbl(k) + 1

  end do

  write ( 3, '(a)' ) '      Rank   Observed  Expected (O-E)^2/E  Sum'

  s = 0.0E+00
  do i = 3, 0, -1
    e = p(i) * real ( ntries )
    d = ( tbl(i) - e )**2 / e
    s = s + d
    write ( 3, '(2i10,f10.1,f10.6,f9.3)' ) n-i, tbl(i), e, d, s
  end do

  write ( 3, '(a)'      ) ' '
  write ( 3, '(a,f6.3)' ) '  Chisquare for 3 degrees of freedom = ', s
  write ( 3, '(a,f8.6)' ) '  P-value = ', chisq ( 3, s )

  jkk = jkreset()

  return
end
subroutine rank32_test ( filename )
!
!*******************************************************************************
!
!! RANK32_TEST performs the rank 32 test.
!
!
!  Discussion:
!
!    This test finds the rank of 32x32 matrices.
!
!    function mrank(r,m,n)
!
!    for nxn matrices, to at least 6 places,
!    the probability of rank n-2,n-1,n are all virtually the same.
!
!    Rank       P
!
!    <=29    0.0052854502
!      30    0.1283502644
!      31    0.5775761902
!      32    0.2887880952
!
  implicit none
!
  integer, parameter :: m = 32
!
  real chisq
  real d
  real e
  character ( len = * ) filename
  integer i
  integer ij
  integer j
  integer jkk
  integer jkreset
  integer jtbl
  integer k
  integer n
  integer, parameter :: ntries = 40000
  integer ntry
  real, dimension (0:3) :: p = (/ &
    0.2887880952E+00, 0.5775761902E+00, 0.1283502644E+00, 0.0052854502E+00 /)
  integer rank
  integer row(m)
  real s
  integer tbl(0:3)

  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'This is the BINARY RANK TEST for 32x32 matrices.'
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'A random 32x32 binary matrix is formed, each row '
  write ( 3, '(a)' ) 'a 32-bit random integer.  The rank is determined. '
  write ( 3, '(a)' ) 'That rank can be from 0 to 32, ranks less than 29 '
  write ( 3, '(a)' ) 'are rare, and their counts are pooled with those'
  write ( 3, '(a)' ) 'for rank 29.  '
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'Ranks are found for 40,000 such random matrices'
  write ( 3, '(a)' ) 'and a chisquare test is performed on counts for '
  write ( 3, '(a)' ) 'ranks  32, 31, 30 and <=29.'

  jkk = jkreset()

  n = m

  write(3,2345) filename
2345    format('    Binary rank test for ',a15)
  write(3,29) m,n
29      format(7x,'  Rank test for ',i2,'x',i2,' binary matrices:')
  write(3,291) m
291     format(7x,' rows from leftmost ',i2, &
       ' bits of each 32-bit integer')

  tbl(0:3) = 0

  do ij = 1, ntries

    do i = 1, m
      row(i) = rshift ( jtbl(), 32 - m )
    end do

    ntry = ntry + 1
    k = rank ( m, n, row )
    k = min ( n - k, 3 )
    tbl(k) = tbl(k) + 1

  end do

  write(3,2457)
2457    format('      rank   observed  expected (o-e)^2/e  sum')

  s = 0.0
  do i = 3, 0, -1
    e = p(i) * real ( ntries )
    d = ( tbl(i) - e )**2 / e
    s = s + d
    write(3,22) n-i,tbl(i),e,d,s
  end do

22  format(2i10,f10.1,f10.6,f9.3)


  write ( 3, '(a)'      ) ' '
  write ( 3, '(a,f6.3)' ) '  Chisquare for 3 degrees of freedom = ', s
  write ( 3, '(a,f8.6)' ) '  P-value = ', chisq ( 3, s )

  jkk = jkreset()

  return
end
function rankb ( m, n, r )
!
!*******************************************************************************
!
!! RANKB determines the rank of a binary matrix.
!
!
!  Parameters:
!
!    Input, integer M, the number of rows in the binary matrix.
!
!    Input, integer N, the number of columns in the binary matrix.
!    Because of the storage scheme for the matrix, N must be no more than 32.
!
!    Input, integer R(M), the binary matrix.  Row I of the matrix
!    consists of the first N bits of R(I).
!
!    Output, integer RANKB, the rank of the matrix.
!
  implicit none
!
  integer m
!
  integer i
  integer ii
  integer j
  integer k
  integer n
  integer, dimension ( 31 ) :: msk = (/ &
           1,        2,         4,         8,        16, &
          32,       64,       128,       256,       512, &
        1024,     2048,      4096,      8192,     16384, &
       32768,    65536,    131072,    262144,    524288, &
     1048576,  2097152,   4194304,   8388608,  16777216, &
    33554432, 67108864, 134217728, 268435456, 536870912, &
    0 /)
  integer r(m)
  integer rankb
  integer x
!
  msk(31) = 32768 * 32768
  rankb = 0
  j = n
  i = 1

33    continue

  do ii = i, m

    if ( and ( r(ii), msk(j) ) == msk(j) ) then

      x = r(ii)
      r(ii) = r(i)
      r(i) = x

      do k = i + 1, m
        if ( and ( r(k), msk(j) ) == msk(j) ) then
          r(k) = xor ( r(k), x )
        end if
      end do

      rankb = rankb + 1

      if ( i == m .or. j == 1 ) then
        return
      end if

      j = j - 1
      i = i + 1
      go to 33

    end if

  end do

  j = j - 1

  if ( j /= 0 ) then
    go to 33
  end if

  return
end
function rank ( m, n, r )
!
!*******************************************************************************
!
!! RANK is almost identical to RANKB...
!
  implicit none
!
! implicit integer(a-z)

  integer i
  integer ii
  integer j
  integer k
  integer, dimension ( 32 ) :: msk = (/ &
           1,        2,         4,         8,        16, &
          32,       64,       128,       256,       512, &
        1024,     2048,      4096,      8192,     16384, &
       32768,    65536,    131072,    262144,    524288, &
     1048576,  2097152,   4194304,   8388608,  16777216, &
    33554432, 67108864, 134217728, 268435456, 536870912, &
           0,        0 /)
  integer m
  integer n
  integer r(32)
  integer rank
  integer x

  msk(31) = 32768 * 32768
  msk(32) = 65536 * 32768
  rank = 0
  j = n
  i = 1

33     continue
!
!  Find row that starts with a 1 in current column (33-j)
!
  do ii = i, m

    if ( and ( r(ii), msk(j) ) == msk(j) ) then

      x = r(ii)
      r(ii) = r(i)
      r(i) = x

      do k = i + 1, m
        if ( and ( r(k), msk(j) ) == msk(j) ) then
          r(k) = xor ( r(k), x )
        end if
      end do

      rank = rank + 1

      if ( i == m .or. j == 1 ) then
        return
      end if

      j = j - 1
      i = i + 1
      go to 33

    end if

  end do

  j = j - 1

  if ( j /= 0 ) then
    go to 33
  end if

  return
end
subroutine r_swap ( x, y )
!
!*******************************************************************************
!
!! R_SWAP swaps two real values.
!
!
!  Modified:
!
!    01 May 2000
!
!  Author:
!
!    John Burkardt
!
!  Parameters:
!
!    Input/output, real X, Y.  On output, the values of X and
!    Y have been interchanged.
!
  implicit none
!
  real x
  real y
  real z
!
  z = x
  x = y
  y = z

  return
end
subroutine runs_test ( filename )
!
!*******************************************************************************
!
!! RUNS_TEST carries out the RUNS test.
!
  implicit none
!
  integer, parameter :: nxs = 10000
!
  real ch
  real dn(100)
  real dv
  character ( len = * ) filename
  integer i
  integer ij
  integer ijkn
  integer j
  integer jkk
  integer jkreset
  integer jtbl
  integer, parameter :: ns = 10
  real p_down
  real p_up
  real up(100)
  real uv
  real x(nxs)
  real xx

  ch ( xx ) = 1.0 - exp ( -0.5 * xx ) * ( 1.0 + 0.5 * xx + 0.125 * xx**2 )

  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'This is the RUNS test.  It counts runs up, and '
  write ( *, '(a)' ) 'runs down, in a sequence of uniform [0,1) '
  write ( *, '(a)' ) 'variables, obtained by floating the 32-bit '
  write ( *, '(a)' ) 'integers in the specified file. '
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'This example shows how runs are counted:  '
  write ( *, '(a)' ) '  0.123, 0.357, 0.789, 0.425, 0.224, 0.416, 0.95'
  write ( *, '(a)' ) 'contains an up-run of length 3, a down-run of '
  write ( *, '(a)' ) 'length 2 and an up-run of (at least) 2, '
  write ( *, '(a)' ) 'depending on the next values.  '
  write ( *, '(a)' ) ' '
  write ( *, '(a)' ) 'The covariance matrices for the runs-up and '
  write ( *, '(a)' ) 'runs-down are well known, leading to chisquare '
  write ( *, '(a)' ) 'tests for quadratic forms in the weak inverses '
  write ( *, '(a)' ) 'of the covariance matrices.  Runs are counted'
  write ( *, '(a)' ) 'for sequences of length 10,000.  This is done '
  write ( *, '(a)' ) 'ten times. Then repeated once.'

  jkk = jkreset()

  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'The RUNS test for file ' // trim ( filename )
  write ( 3, '(a)' ) '  Up and down runs in a sample of 10000.'

  do ijkn = 1, 2

    do ij = 1, ns

      do i = 1, nxs
        x(i) = jtbl() * 2.328306E-10
      end do

      call udruns ( nxs, x, uv, dv )

      up(ij) = ch(uv)
      dn(ij) = ch(dv)

    end do

    call ks_test ( ns, up, p_up )

    call ks_test ( ns, dn, p_down )

    write ( 3, '(a,f8.6,a,f8.6)' ) &
      '  Runs up KS        = ', p_up, &
      '  Runs down KS test = ', p_down

  end do

  jkk = jkreset()

  return
end
subroutine rvec_heap_d ( n, a )
!
!*******************************************************************************
!
!! RVEC_HEAP_D reorders an array of reals into an descending heap.
!
!
!  Definition:
!
!    A descending heap is an array A with the property that, for every index J,
!    A(J) >= A(2*J) and A(J) >= A(2*J+1), (as long as the indices
!    2*J and 2*J+1 are legal).
!
!  Diagram:
!
!                  A(1)
!                /      \
!            A(2)         A(3)
!          /     \        /  \
!      A(4)       A(5)  A(6) A(7)
!      /  \       /   \
!    A(8) A(9) A(10) A(11)
!
!  Modified:
!
!    07 July 2003
!
!  Author:
!
!    John Burkardt
!
!  Reference:
!
!    A Nijenhuis and H Wilf,
!    Combinatorial Algorithms,
!    Academic Press, 1978, second edition,
!    ISBN 0-12-519260-6.
!
!  Parameters:
!
!    Input, integer N, the size of the input array.
!
!    Input/output, real A(N).
!    On input, an unsorted array.
!    On output, the array has been reordered into a heap.
!
  implicit none
!
  integer n
!
  real a(n)
  integer i
  integer ifree
  real key
  integer m
!
!  Only nodes N/2 down to 1 can be "parent" nodes.
!
  do i = n/2, 1, -1
!
!  Copy the value out of the parent node.
!  Position IFREE is now "open".
!
    key = a(i)
    ifree = i

    do
!
!  Positions 2*IFREE and 2*IFREE + 1 are the descendants of position
!  IFREE.  (One or both may not exist because they exceed N.)
!
      m = 2 * ifree
!
!  Does the first position exist?
!
      if ( n < m ) then
        exit
      end if
!
!  Does the second position exist?
!
      if ( m + 1 <= n ) then
!
!  If both positions exist, take the larger of the two values,
!  and update M if necessary.
!
        if ( a(m) < a(m+1) ) then
          m = m + 1
        end if

      end if
!
!  If the large descendant is larger than KEY, move it up,
!  and update IFREE, the location of the free position, and
!  consider the descendants of THIS position.
!
      if ( a(m) <= key ) then
        exit
      end if

      a(ifree) = a(m)
      ifree = m

    end do
!
!  Once there is no more shifting to do, KEY moves into the free spot IFREE.
!
    a(ifree) = key

  end do

  return
end
subroutine rvec_sort_heap_a ( n, a )
!
!*******************************************************************************
!
!! RVEC_SORT_HEAP_A ascending sorts a real array using heap sort.
!
!
!  Modified:
!
!    07 July 2003
!
!  Author:
!
!    John Burkardt
!
!  Reference:
!
!    A Nijenhuis and H Wilf,
!    Combinatorial Algorithms,
!    Academic Press, 1978, second edition,
!    ISBN 0-12-519260-6.
!
!  Parameters:
!
!    Input, integer N, the number of entries in the array.
!
!    Input/output, real A(N).
!    On input, the array to be sorted;
!    On output, the array has been sorted.
!
  implicit none
!
  integer n
!
  real a(n)
  integer n1
!
  if ( n <= 1 ) then
    return
  end if
!
!  1: Put A into descending heap form.
!
  call rvec_heap_d ( n, a )
!
!  2: Sort A.
!
!  The largest object in the heap is in A(1).
!  Move it to position A(N).
!
  call r_swap ( a(1), a(n) )
!
!  Consider the diminished heap of size N1.
!
  do n1 = n-1, 2, -1
!
!  Restore the heap structure of A(1) through A(N1).
!
    call rvec_heap_d ( n1, a )
!
!  Take the largest object from A(1) and move it to A(N1).
!
    call r_swap ( a(1), a(n1) )

  end do

  return
end
subroutine spheres_test ( filename )
!
!*******************************************************************************
!
!! SPHERES_TEST applies the 3D spheres test.
!
  implicit none
!
  integer, parameter :: n = 4000
!
  real d
  real dmin
  character ( len = * ) filename
  integer i
  integer ij
  integer j
  integer jkk
  integer jkreset
  integer jtbl
  real p(20)
  real pv
  real r3
  real u
  real uni
  real v
  real w
  real x(n)
  real y(n)
  real z(n)

  uni() = 500.0 + 0.2328306E-06 * jtbl()

  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'THE 3DSPHERES TEST'
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'Choose 4000 random points in a cube of edge '
  write ( 3, '(a)' ) '1000.  At each point, center a sphere large '
  write ( 3, '(a)' ) 'enough to reach the next closest point. Then '
  write ( 3, '(a)' ) 'the volume of the smallest such sphere is (very' 
  write ( 3, '(a)' ) 'close to) exponentially distributed with mean '
  write ( 3, '(a)' ) '120 pi / 3.  Thus the radius cubed is exponential '
  write ( 3, '(a)' ) 'with mean 30. (The mean is obtained by extensive '
  write ( 3, '(a)' ) 'simulation).  The 3DSPHERES test generates 4000 '
  write ( 3, '(a)' ) 'such spheres 20 times.  Each minimum radius cubed '
  write ( 3, '(a)' ) 'leads to a uniform variable by means of '
  write ( 3, '(a)' ) '1-exp(-r^3/30.), then a KS test is done on the '
  write ( 3, '(a)' ) '20 p-values. '

  jkk = jkreset()

  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'The 3DSPHERES test for file ' // filename

  do ij = 1, 20

    dmin = huge ( dmin )

    do i = 1, n
      x(i) = uni()
    end do

    call rvec_sort_heap_a ( n, x )

    do i = 1, n
      y(i) = uni()
      z(i) = uni()
    end do

    do i = 1, n

      u = x(i)
      v = y(i)
      w = z(i)

      do j = i + 1, n
        d = ( u - x(j) )**2
        if ( dmin <= d ) then
          exit
        end if
        d = d + ( v - y(j) )**2 + ( w - z(j) )**2
        dmin = min ( dmin, d )
      end do

    end do

    r3 = dmin * sqrt ( dmin )
    p(ij) = 1.0 - exp ( - r3 / 30.0 )

    write( 3, '(a,i2,a,f7.3,a,f7.5)' ) &
      '  Sample number = ', ij, &
      '  R^3 = ', r3, &
      '  P value = ', p(ij)

  end do

  call ks_test ( 20, p, pv )

  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) '  A KS test is applied to those 20 P-values.'
  write ( 3, '(a,f8.6)' ) '  P-value for 3DSPHERES test = ', pv

  jkk = jkreset()

  return
end
subroutine squeeze_test ( filename )
!
!*******************************************************************************
!
!! SQUEEZE_TEST applies the squeeze test.
!
!
!  Discussion:
!
!    The Squeeze test examines how many iterations of
!      k = k * uni() + 1
!    are required to squeeze k down to 1, starting with
!      k = 2147483647 = 2^31-1.
!
!    The exact distribution of the required J is used, with
!    a chi-square test based on 100,000 tries.
!
!    The mean of J is 23.064779, with variance 23.70971151.
!
  implicit none
!
  real chisq
  real chsq
  real, dimension (6:48) :: ex = (/ &
        21.03,    57.79,   175.54,   467.32,  1107.83, &
      2367.84,  4609.44,  8241.16, 13627.81, 20968.49, &
     30176.12, 40801.97, 52042.03, 62838.28, 72056.37, &
     78694.51, 82067.55, 81919.35, 78440.08, 72194.12, &
     63986.79, 54709.31, 45198.52, 36136.61, 28000.28, &
     21055.67, 15386.52, 10940.20,  7577.96,  5119.56, &
      3377.26,  2177.87,  1374.39,   849.70,   515.18, &
       306.66,   179.39,   103.24,    58.51,    32.69, &
        18.03,     9.82,    11.21 /)
  character ( len = * ) filename
  integer i
  integer j
  integer jkk
  integer jkreset
  integer jtbl
  integer k
  real sig
  real tbl(6:48)
  real uni
!
!  Put a one-line function here to provide the uni being tested:
!
  uni() = 0.5 + jtbl() * 0.5**32

  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'This is the SQUEEZE test'
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'Random integers are floated to get uniforms '
  write ( 3, '(a)' ) 'on [0,1).  Starting with K = 2^31 = 2147483647, '
  write ( 3, '(a)' ) 'the test finds J, the number of iterations '
  write ( 3, '(a)' ) 'necessary to reduce K to 1, using the reduction'
  write ( 3, '(a)' ) '  k = ceiling ( k * U ), '
  write ( 3, '(a)' ) 'with U provided by floating integers from '
  write ( 3, '(a)' ) 'the file being tested.  Such J''s are found '
  write ( 3, '(a)' ) '100,000 times, then counts for the number of '
  write ( 3, '(a)' ) 'times J was <= 6, 7,...,47, 48 or more' 
  write ( 3, '(a)' ) 'are used to provide a chi-square test for cell '
  write ( 3, '(a)' ) 'frequencies.'

  jkk = jkreset()

  tbl(6:48) = 0

  do i = 1, 100000

    j = 0
    k = 2147483647

    do

      k = k * uni() + 1
      j = j + 1

      if ( k <= 1 ) then
        exit
      end if

    end do

    j = max ( j, 6 )
    j = min ( j, 48 )

    tbl(j) = tbl(j) + 1.0

  end do

  chsq = 0.0
  do i = 6, 48
    chsq = chsq + ( tbl(i) - 0.1 * ex(i) )**2 / ( 0.1 * ex(i) )
  end do

  sig = sqrt ( 84.0 )

  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) 'The Squeeze test for ' // trim ( filename )
  write ( 3, '(a)' ) ' '
  write ( 3, '(a)' ) '  Table of standardized frequency counts'
  write ( 3, '(a)' ) '  ( (obs-exp)/sqrt(exp) )^2'
  write ( 3, '(a)' ) '  for j taking values < = 6,7,8,...,47,>=48:'
  write ( 3, '(a)' ) ' '
  write ( 3, '(6f8.1)' ) &
    ( tbl(6:48) - 0.1 * ex(6:48) ) / sqrt ( 0.1 * ex(6:48) )
  write ( 3, '(a)' ) ' '
  write ( 3, '(a,f7.3)' ) '  Chi-square with 42 degrees of freedom: ', chsq
  write ( 3, '(a,f7.3)' ) '  Z-score = ', ( chsq - 42.0 ) / sig
  write ( 3, '(a,f8.6)' ) '  P-value = ', chisq ( 42, chsq )

  jkk = jkreset()

  return
end
subroutine timestamp ( )
!
!*******************************************************************************
!
!! TIMESTAMP prints the current YMDHMS date as a time stamp.
!
!
!  Example:
!
!    May 31 2001   9:45:54.872 AM
!
!  Modified:
!
!    15 March 2003
!
!  Author:
!
!    John Burkardt
!
!  Parameters:
!
!    None
!
  implicit none
!
  character ( len = 40 ) string
!
  call timestring ( string )

  write ( *, '(a)' ) trim ( string )

  return
end
subroutine udruns ( n, x, uv, dv )
!
!*******************************************************************************
!
!! UDRUNS computes the runs-up and runs-down in a sequence.
!
!
!  Discussion:
!
!    The routine counts the number of runs up (and down) of lengths 1
!    through 5, with a final count for runs of 6 or greater.
!
!    It then calculates the test statistics.
!
!  Reference:
!
!    Algorithm AS 157
!    Applied Statistics
!    Volume 30, Number 1, 1981
!
!  Parameters:
!
!    Input, integer N, the length of the sequence.
!
!    Input, real X(N), the sequence to examine.
!
!    Output, real UV, DV, the runs up and runs down statistics.
!
  implicit none
!
  integer n
!
  real, dimension (6,6) :: a = reshape ( &
    (/ 4529.4,  9044.9, 13568.0,  18091.0,  22615.0,  27892.0, &
       9044.9, 18097.0, 27139.0,  36187.0,  45234.0,  55789.0, &
      13568.0, 27139.0, 40721.0,  54281.0,  67852.0,  83685.0, &
      18091.0, 36187.0, 54281.0,  72414.0,  90470.0, 111580.0, &
      22615.0, 45234.0, 67852.0,  90470.0, 113262.0, 139476.0, &
      27892.0, 55789.0, 83685.0, 111580.0, 139476.0, 172860.0 /), &
    (/ 6, 6 /) )
  real, dimension (6) :: b = (/ &
    1.0 / 6.0, 5.0 / 24.0, 11.0 / 120.0, 19.0 / 720.0, 29.0 / 5040.0, &
    1.0 / 840.0 /)
  integer dcount(6)
  real dv
  integer i
  integer j
  integer rd
  real rn
  integer ru
  integer ucount(6)
  real uv
  real x(n)

  ucount(1:6) = 0
  dcount(1:6) = 0
!
!  The loop determines the number of
!  runs-up and runs-down of length i for i = 1(1)5 and the number
!  of runs-up and runs-down of length greater than or equal to 6.
!
  ru = 1
  rd = 1

  do j = 2, n

    if ( x(j) <= x(j-1) ) then

      ucount(ru) = ucount(ru) + 1

      ru = 1

      if ( rd < 6 ) then
        rd = rd + 1
      end if

    else

      dcount(rd) = dcount(rd) + 1

      rd = 1

      if ( ru < 6 ) then
        ru =  ru + 1
      end if

    end if

  end do

  ucount(ru) = ucount(ru) + 1
  dcount(rd) = dcount(rd) + 1
!
!  Calculate the test statistics UV and DV.
!
  uv = 0.0
  dv = 0.0

  do i = 1, 6
    do j =  1, 6
      uv = uv + ( real ( ucount(i) ) - real ( n ) * b(i) ) * &
                ( real ( ucount(j) ) - real ( n ) * b(j) ) * a(i,j)
      dv = dv + ( real ( dcount(i) ) - real ( n ) * b(i) ) * &
                ( real ( dcount(j) ) - real ( n ) * b(j) ) * a(i,j)
    end do
  end do

  uv = uv / real ( n )
  dv = dv / real ( n )

  return
end
