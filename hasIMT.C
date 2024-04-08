/// Ensure if IMT is enabled

void enable_imt() {
std::cout << R__HAS_IMT << std::endl;

#ifdef R__HAS_IMT
    // Enable ROOT's Implicit Multi-Threading (IMT)
    std::cout << "Already enabled IMT." << std::endl;
#else
    ROOT::EnableImplicitMT();
    std::cout << "IMT is now enabled." << std::endl;

#endif
std::cout << R__HAS_IMT << std::endl;

}