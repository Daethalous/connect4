class Rotor:
    def __init__(self, name, wiring, turnover_notch):
        self.name = name
        self.wiring = wiring
        self.turnover_notch = turnover_notch
        self.ring_setting = 'A'
        self.position = 'A'

    def set_ring_setting(self, setting):
        self.ring_setting = setting

    def set_initial_position(self, position):
        self.position = position

    def rotate(self):
        self.position = chr((ord(self.position) - ord('A') + 1) % 26 + ord('A'))

    def forward(self, char):
        offset = ord(self.position) - ord(self.ring_setting)
        alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
        index = (ord(self.wiring[(offset + ord(char) - ord('A')) % 26]) - ord('A') - offset) % 26
        return alphabet[index]

    def backward(self, char):
        offset = ord(self.position) - ord(self.ring_setting)
        alphabet = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
        char = alphabet[(ord(char) - ord('A') + offset) % 26]
        for i in range(len(self.wiring)):
            if self.wiring[i] == char:
                index = (i - offset) % 26
                return alphabet[index]

class Plugboard:
    def __init__(self):
        self.connections = {}

    def add_connection(self, letter1, letter2):
        self.connections[letter1] = letter2
        self.connections[letter2] = letter1

    def process(self, char):
        return self.connections.get(char, char)
               
class Reflector:
    def __init__(self, wiring):
        self.wiring = wiring

    def reflect(self, char):
        return self.wiring[ord(char) - ord('A')]
    
class EnigmaMachine:
    def __init__(self, rotor1, rotor2, rotor3, plugboard):
        self.reflector = Reflector(['Y','R','U','H','Q','S','L','D','P','X','N','G','O','K','M','I','E','B','F','Z','C','W','V','J','A','T'])
        self.rotors = [rotor1, rotor2, rotor3]
        self.plugboard = plugboard

    def set_rotor_positions(self, positions):
        if len(positions) != len(self.rotors):
            print("错误：位置数量与转子数量不匹配。")
            return
        for rotor, position in zip(self.rotors, positions):
            rotor.set_initial_position(position)

    def set_rotor_ring_settings(self, settings):
        if len(settings) != len(self.rotors):
            print("错误：环数量与转子数量不匹配。")
            return
        for rotor, setting in zip(self.rotors, settings):
            rotor.set_ring_setting(setting)

    def reset_plugboard(self, plugboard):
        self.plugboard = plugboard
    
    def check1(self, char):
        for _ in range(5):
            self.rotate_rotors
        new_char = self.encrypt(char)
        for _ in range(2):
            self.rotate_rotors
        new_char = self.encrypt(new_char)
        return new_char == char
    
    def check2(self, char):
        for _ in range(13):
            self.rotate_rotors
        new_char = self.encrypt(char)
        for _ in range(1):
            self.rotate_rotors
        new_char = self.encrypt(new_char)
        return new_char == char
    
    def check3(self, char):
        new_char_1 = self.encrypt(char)
        for _ in range(12):
            self.rotate_rotors
        new_char_2 = self.encrypt(char)
        for _ in range(17):
            self.rotate_rotors
        new_char_2 = self.encrypt(new_char_2)
        return new_char_1 == new_char_2
    

    def rotate_rotors(self):
        if self.rotors[1].position in self.rotors[1].turnover_notch:
            self.rotors[1].rotate()
            self.rotors[0].rotate()
        if self.rotors[2].position in self.rotors[2].turnover_notch:
            self.rotors[1].rotate()
        self.rotors[2].rotate()
        
    def encrypt(self, message):
        encrypted_message = ""
        for char in message:
            char = self.plugboard.process(char)
            self.rotate_rotors()
            encrypted_char = char
            for rotor in reversed(self.rotors):
                encrypted_char = rotor.forward(encrypted_char)
            encrypted_char = self.reflector.reflect(encrypted_char)
            for rotor in self.rotors:
                encrypted_char = rotor.backward(encrypted_char)
            encrypted_char = self.plugboard.process(encrypted_char)
            encrypted_message += encrypted_char
        return encrypted_message
    
def main():
    rotorI = Rotor('I', ['E','K','M','F','L','G','D','Q','V','Z','N','T','O','W','Y','H','X','U','S','P','A','I','B','R','C','J'], 'Q')
    rotorII = Rotor('II', ['A','J','D','K','S','I','R','U','X','B','L','H','W','T','M','C','Q','G','Z','N','P','Y','F','V','O','E'], 'E')
    rotorIII = Rotor('III', ['B','D','F','H','J','L','C','P','R','T','X','V','Z','N','Y','E','I','W','G','A','K','M','U','S','Q','O'], 'V')

    plugboard = Plugboard()
    plugboard.add_connection('B', 'X')
    plugboard.add_connection('G', 'K')
    plugboard.add_connection('W', 'Y')
    plugboard.add_connection('E', 'F')
    plugboard.add_connection('P', 'Q')
    plugboard.add_connection('S', 'N')

    rotor_order = input("rotor sequence: ").split()

    rotor_mapping = {'I': rotorI, 'II': rotorII, 'III': rotorIII}

    rotors = [rotor_mapping[rotor_name] for rotor_name in rotor_order]
    
    enigma_machine = EnigmaMachine(*rotors, plugboard)

    positions = input("initial position: ").split()
    enigma_machine.set_rotor_positions(positions)

    ring_settings = input("ring setting: ").split()
    enigma_machine.set_rotor_ring_settings(ring_settings)

    message = input("input: ")
    encrypted_message = enigma_machine.encrypt(message)
    print("output:", encrypted_message)

if __name__ == "__main__":
    main()