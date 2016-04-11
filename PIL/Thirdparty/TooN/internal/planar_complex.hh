namespace TooN{
	namespace Internal{
		
		///@internal
		///@brief A simple smart pointer type representing planar complex data.
		///Not resizable. Also, only the minimum number of smart pointer
		///functionality has been implemented to make Vector work. The class returns
		///reference types, so it can represent mutable data.
		///@ingroup gInternal
		template<class Precision> struct PointerToPlanarComplex;
		template<class Precision> struct PointerToPlanarComplex<std::complex<Precision> >
		{
			const Precision* const re;
			const Precision* const im;
		
			PointerToPlanarComplex(std::pair<Precision*, Precision*> d)
			:re(d.first),im(d.second)
			{}

			PointerToPlanarComplex(std::pair<const Precision*, const Precision*> d)
			:re(d.first),im(d.second)
			{}

			PointerToPlanarComplex<std::complex<Precision> > operator+(int i) const
			{
				return PointerToPlanarComplex<std::complex<Precision> >(std::make_pair(re+i, im+i));
			}

			const std::complex<Precision> operator[](int i) const
			{
				return std::complex<Precision>(re[i], im[i]);
			}

		};
	}

	
	struct ReferencePlanarComplex
	{
		template<int Size, typename Precision>
		struct VLayout;

		template<int Size, typename Precision>
		struct VLayout<Size, std::complex<Precision> >: 
		  public Internal::GenericVBase<Size, std::complex<Precision>, 1, Internal::VectorSlice<Size, std::complex<Precision>, Internal::PointerToPlanarComplex<std::complex<double> >,
	                                                    Internal::PointerToPlanarComplex<std::complex<double> >,
	                                                    const std::complex<double>,
	                                                    const std::complex<double> > >
		{
			VLayout(Internal::PointerToPlanarComplex<std::complex<Precision> > p, int sz=0)
			:Internal::GenericVBase<Size, std::complex<Precision>, 1, Internal::VectorSlice<Size, std::complex<Precision>, Internal::PointerToPlanarComplex<std::complex<double> >,
	                                                    Internal::PointerToPlanarComplex<std::complex<double> >,
	                                                    const std::complex<double>,
	                                                    const std::complex<double> > >(p, sz, 1)
			{}

			
		};

	};
}

