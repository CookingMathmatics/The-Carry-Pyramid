#include <iostream>
#include <vector>
#include <cmath>
#include <queue>
#include <utility>
#include <string>
#include <sstream>
#include <iomanip>

const int LATTICE_SIZE = 121;
const int CARRY_BASE = 2;

// 1. 비선형 카오스 믹서
std::pair<int, int> chaos_mixer(double raw_sensor_value) {
    double chaotic = std::sin(raw_sensor_value * 12345.67) * std::cos(raw_sensor_value * 7654.32);
    chaotic = std::abs(chaotic) - std::floor(std::abs(chaotic));
    int x = static_cast<int>(chaotic * 10000) % LATTICE_SIZE;
    int y = static_cast<int>(chaotic * 100000) % LATTICE_SIZE;
    return {x, y};
}

// 2. p-adic 순방향 캐리 전파 엔진
int simulate_carry_avalanche(std::vector<std::vector<int>>& lattice, int start_x, int start_y) {
    std::queue<std::pair<int, int>> carry_queue;
    lattice[start_x][start_y] += 33; // 대칭을 깨뜨려 카오스 파편을 생성
    carry_queue.push({start_x, start_y});
    
    int total_carries = 0;
    int dx[] = {1, 0};
    int dy[] = {0, 1};
    
    while (!carry_queue.empty()) {
        auto [cx, cy] = carry_queue.front();
        carry_queue.pop();
        
        if (lattice[cx][cy] >= CARRY_BASE) {
            int carry_amount = lattice[cx][cy] / CARRY_BASE;
            lattice[cx][cy] %= CARRY_BASE; 
            total_carries++;
            
            for (int i = 0; i < 2; ++i) {
                int nx = cx + dx[i];
                int ny = cy + dy[i];
                
                if (nx < LATTICE_SIZE && ny < LATTICE_SIZE) {
                    lattice[nx][ny] += carry_amount;
                    if (lattice[nx][ny] >= CARRY_BASE) {
                        carry_queue.push({nx, ny});
                    }
                }
            }
        }
    }
    return total_carries;
}

// 3. [완벽 복구] 글로벌 격자 압축 암호 키 추출 로직
// 특정 구역만 보는 것이 아니라, 121x121 전체 격자판의 세포 상태를 256비트 암호 블록에 골고루 확산(Diffusion)시킵니다.
std::string extract_global_crypto_key(const std::vector<std::vector<int>>& lattice) {
    // 암호학적 초기 벡터(SHA-256 상수 활용) 세팅
    unsigned int hash_blocks[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };
    
    // 격자판 전체를 순회하며 암호 블록을 비선형적으로 휘감기(Rolling Hash)
    for (int i = 0; i < LATTICE_SIZE; ++i) {
        for (int j = 0; j < LATTICE_SIZE; ++j) {
            int val = lattice[i][j];
            int block_idx = (i * LATTICE_SIZE + j) % 8;
            
            // FNV-1a 스타일의 비트 소용돌이 연산 적용
            hash_blocks[block_idx] = (hash_blocks[block_idx] ^ val) * 16777619;
            hash_blocks[(block_idx + 1) % 8] ^= (val * 31);
        }
    }
    
    // 8개의 32비트 블록을 16진수 문자열로 조합하여 최종 64글자(256비트) 키 생성
    std::stringstream ss;
    for (int i = 0; i < 8; ++i) {
        ss << std::setw(8) << std::setfill('0') << std::hex << hash_blocks[i];
    }
    return ss.str();
}

int main() {
    std::cout << "==================================================\n";
    std::cout << "       Physical Ghost 가상 PoC 실험실 v3.5 (통합)\n";
    std::cout << "==================================================\n\n";

    std::vector<std::vector<int>> lattice(LATTICE_SIZE, std::vector<int>(LATTICE_SIZE, 0));

    // 🎯 [실험 가이드] 아래 센서 값을 1.4589와 1.4590으로 번갈아 바꾸며 테스트해 보세요!
    double mock_analog_input = 1.4590; 
    
    // 단계 1: 믹서 가동
    auto [target_x, target_y] = chaos_mixer(mock_analog_input);
    std::cout << "[+] [단계 1] 센서 데이터 [" << mock_analog_input << "V] -> 좌표 매핑: (" << target_x << ", " << target_y << ")\n";

    // 단계 2: 도미노 엔진 가동
    int avalanche_count = simulate_carry_avalanche(lattice, target_x, target_y);
    std::cout << "[+] [단계 2] p-adic 캐리 도미노 엔진 가동 -> 총 [" << avalanche_count << "] 번의 폭발 완료.\n";
    
    // 단계 3: 글로벌 키 추출
    std::cout << "[>] [단계 3] 121x121 전체 위상 지형에서 글로벌 마스터 키 추출 중...\n\n";
    std::string final_crypto_key = extract_global_crypto_key(lattice);
    
    // 최종 결과 리포트
    std::cout << "======================= Physical Ghost 마스터 키 =======================\n";
    std::cout << " 0x" << final_crypto_key << "\n";
    std::cout << "========================================================================\n";
    std::cout << "[성공] 전체 격자 동역학 스캔 기반 AES-256 규격 암호 키 생성 완료.\n";

    return 0;
}