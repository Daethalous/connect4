from enigma import Rotor, Plugboard, Reflector, EnigmaMachine
import itertools

def main():
    rotorI = Rotor('I', ['E','K','M','F','L','G','D','Q','V','Z','N','T','O','W','Y','H','X','U','S','P','A','I','B','R','C','J'], 'Q')
    rotorII = Rotor('II', ['A','J','D','K','S','I','R','U','X','B','L','H','W','T','M','C','Q','G','Z','N','P','Y','F','V','O','E'], 'E')
    rotorIII = Rotor('III', ['B','D','F','H','J','L','C','P','R','T','X','V','Z','N','Y','E','I','W','G','A','K','M','U','S','Q','O'], 'V')

    rotor_mapping = {'I': rotorI, 'II': rotorII, 'III': rotorIII}

    plugboard = Plugboard()
    for rotor_order in itertools.permutations(rotor_mapping.keys()):
        rotors = [rotor_mapping[rotor_name] for rotor_name in rotor_order]
        enigma_machine = EnigmaMachine(*rotors, plugboard)
        enigma_machine.set_rotor_ring_settings("DES")
        for initial_positions in itertools.product("ABCDEFGHIJKLMNOPQRSTUVWXYZ", repeat=3):
            enigma_machine.set_rotor_positions(initial_positions)
            for s_a in itertools.cycle("BCDEFGHIJKLMNOPQRSTUVWXYZ"):
                plugboard = Plugboard()
                plugboard.add_connection('A', s_a)
                enigma_machine = EnigmaMachine(*rotors, plugboard)
                if enigma_machine.check1(s_a):
                    for s_t in itertools.cycle("ABCDEFGHIJKLMNOPQRSUVWXYZ"):
                        plugboard.add_connection('T', s_t)
                        enigma_machine = EnigmaMachine(*rotors, plugboard)
                        if enigma_machine.check2(s_t):
                            for s_e in itertools.cycle("ABCDFGHIJKLMNOPQRSTUVWXYZ"):
                                plugboard = Plugboard()
                                plugboard.add_connection('E', s_e)
                                enigma_machine = EnigmaMachine(*rotors, plugboard)
                                if enigma_machine.check3(s_e):
                                    enigma_machine = EnigmaMachine(*rotors, plugboard)
                                    if enigma_machine.encrypt("ENIGM ADAIL YSETT INGS") == "LVQUK OJOWP NTGHH NGLZ":
                                        print("找到匹配:")
                                        print("rotor sequence:", ' '.join(rotor_order))
                                        print("initial position:", ' '.join(initial_positions))
    print("未找到匹配")

if __name__ == "__main__":
    main()
