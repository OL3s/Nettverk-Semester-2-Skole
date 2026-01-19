#include <iostream>
#include <thread>
#include <vector>
#include <fstream>

static bool is_prime(long long n) {
	if (n < 2) {
		return false;
	}
	if (n % 2 == 0) {
		return n == 2;
	}
	for (long long d = 3; d * d <= n; d ++) {
		if (n % d == 0) {
			return false;
		}
	}
	return true;
}

int main(int argc, char* argv[]) {

    // === ERROR INPUT ===
    // missing arguments
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " missing arguments <from> <to> <num_threads> [<output_to_file> = true]" << std::endl;
        return 1;
    }

    // invalid range
    if (std::stoll(argv[1]) >= std::stoll(argv[2])) {
        std::cerr << "Error: <from> must be less than <to>." << std::endl;
        return 1;
    }

    // === INITIATE ===
    // arguments
    bool output_to_file = true;
    if (argc > 4) {
        output_to_file = (std::string(argv[4]) == "true") || (std::string(argv[4]) == "1");
    }

    long long from = std::stoll(argv[1]);
    long long to = std::stoll(argv[2]);
    int num_threads = std::stoi(argv[3]);
    
    // calculated values
    int interval = (to - from) / num_threads;
    long startPositions[num_threads];
    std::thread t[num_threads];
    std::vector<std::vector<long long>> primes(num_threads);

    // insert start positions for each thread
    for (int i = 0; i < num_threads; i++) {
        startPositions[i] = from + i * interval;
    }

    // === RUN THREADS ===
    // thread calculate primes
    for (int i = 0; i < num_threads; i++) {
        t[i] = std::thread([&, i]() {
            std::vector<long long> localPrimes;
            long long start = startPositions[i];
            long long end = (i == num_threads - 1) ? to : startPositions[i] + interval;
            for (long long n = start; n < end; n++) {
                if (is_prime(n)) {
                    localPrimes.push_back(n);
                }
            }
            primes[i] = localPrimes;
        });
    }

    // join threads before output
    for (int i = 0; i < num_threads; i++) {
        t[i].join();
    }

    // === OUTPUT RESULTS ===
    if (output_to_file) {
        std::ofstream outfile("primes.txt");

        // file error check
        if (!outfile.is_open()) {
            std::cerr << "Error opening file for writing." << std::endl;
            return 1;
        }

        // write primes to file
        outfile << "Primes [" << from << " - " << to << "]; threads: " << num_threads << std::endl;
        int count = 0;
        for (int i = 0; i < num_threads; i++) {
            for (long long prime : primes[i]) {
                outfile << prime << ", "; count++; 
                if (count % 10 == 0) outfile << std::endl;
            }
        }
        outfile << "\nPrimes [" << from << " - " << to << "]; threads: " << num_threads << std::endl;
        outfile.close();
        return 0;

    // === OUTPUT TO CONSOLE ===
    } else {    
        std::cout << "Primes [" << from << " - " << to << "]; threads: " << num_threads << std::endl;
        int count = 0;
        for (int i = 0; i < num_threads; i++) {
            for (long long prime : primes[i]) {
                std::cout << prime << ", "; count++; 
                if (count % 10 == 0) std::cout << std::endl;
            }
        }
        std::cout << "\nPrimes [" << from << " - " << to << "]; threads: " << num_threads << std::endl;
        return 0;
    }
}
