# 密码学 第一次作业
刘蕴皓 2021012884
## 一、古典密码部分
1. 采用穷举，得出以下结果：
```
Shift 1: PFLDLJKSVKYVTYREXVPFLNZJYKFJVVZEKYVNFICU
Shift 2: OEKCKIJRUJXUSXQDWUOEKMYIXJEIUUYDJXUMEHBT
Shift 3: NDJBJHIQTIWTRWPCVTNDJLXHWIDHTTXCIWTLDGAS
Shift 4: MCIAIGHPSHVSQVOBUSMCIKWGVHCGSSWBHVSKCFZR
Shift 5: LBHZHFGORGURPUNATRLBHJVFUGBFRRVAGURJBEYQ
Shift 6: KAGYGEFNQFTQOTMZSQKAGIUETFAEQQUZFTQIADXP
Shift 7: JZFXFDEMPESPNSLYRPJZFHTDSEZDPPTYESPHZCWO
Shift 8: IYEWECDLODROMRKXQOIYEGSCRDYCOOSXDROGYBVN
Shift 9: HXDVDBCKNCQNLQJWPNHXDFRBQCXBNNRWCQNFXAUM
Shift 10: GWCUCABJMBPMKPIVOMGWCEQAPBWAMMQVBPMEWZTL
Shift 11: FVBTBZAILAOLJOHUNLFVBDPZOAVZLLPUAOLDVYSK
Shift 12: EUASAYZHKZNKINGTMKEUACOYNZUYKKOTZNKCUXRJ
Shift 13: DTZRZXYGJYMJHMFSLJDTZBNXMYTXJJNSYMJBTWQI
Shift 14: CSYQYWXFIXLIGLERKICSYAMWLXSWIIMRXLIASVPH
Shift 15: BRXPXVWEHWKHFKDQJHBRXZLVKWRVHHLQWKHZRUOG
Shift 16: AQWOWUVDGVJGEJCPIGAQWYKUJVQUGGKPVJGYQTNF
Shift 17: ZPVNVTUCFUIFDIBOHFZPVXJTIUPTFFJOUIFXPSME
Shift 18: YOUMUSTBETHECHANGEYOUWISHTOSEEINTHEWORLD
Shift 19: XNTLTRSADSGDBGZMFDXNTVHRGSNRDDHMSGDVNQKC
Shift 20: WMSKSQRZCRFCAFYLECWMSUGQFRMQCCGLRFCUMPJB
Shift 21: VLRJRPQYBQEBZEXKDBVLRTFPEQLPBBFKQEBTLOIA
Shift 22: UKQIQOPXAPDAYDWJCAUKQSEODPKOAAEJPDASKNHZ
Shift 23: TJPHPNOWZOCZXCVIBZTJPRDNCOJNZZDIOCZRJMGY
Shift 24: SIOGOMNVYNBYWBUHAYSIOQCMBNIMYYCHNBYQILFX
Shift 25: RHNFNLMUXMAXVATGZXRHNPBLAMHLXXBGMAXPHKEW
```
观察到其中移18位的结果可读，成功破解，明文如下
> YOUMUSTBETHECHANGEYOUWISHTOSEEINTHEWORLD



2. 统计各字母出现频率：

> 字母    频率
D       0.1255
B       0.0867
W       0.0828
S       0.0776
O       0.0724
……

推测S(o)=D，根据双字频率推测S(n)=O，S(a)=W。

根据密文GDAWG及高频出现的WOG推测S(d)=G。

以此类推，可以推测出that recent been secret等词。

最终解出明文：

> Cryptography, the use of codes and ciphers to protect secrets, began thousands of years ago. Until recent decades, it has been the story of what might be called classical cryptography. That is, of methods of encryption that use pen and paper, or perhaps simple mechanical aids. In the early twentieth century, the invention of complex mechanical and electromechanical machines, such as the Enigma rotor machine, provided more sophisticated and efficient means of encryption. The subsequent introduction of electronics and computing has allowed elaborate schemes of still greater complexity, most of which are entirely unsuited to pen and paper.The development of cryptography has been paralleled by the development of cryptanalysis, that is, the breaking of codes and ciphers. The discovery and application early on of frequency analysis to the reading of encrypted communications has on occasion altered the course of history.



3. 观察密文中出现频率最高的三字符串：

   'POC': [90, 135, 420]

   'GUG': [124, 334, 409, 504]

   'UGP': [125, 335, 410]

   'IWC': [247, 287, 487]

   'PLK': [258, 443, 493]

   可以推测密钥长度t=5。



4. enigma加密和破解

   1. enigma模拟器

      在enigma.py中定义Rotor,Plugboard,Reflector,定义其输入输出方法。

      定义EnigmaMachine类，组装上述部件并定义初始化设置方法，用于设置转子顺序、初始位置等。

      参考题目给出的设置运行结果如下：
      ```
      rotor sequence: II III I
      initial position: A A A
      ring setting: D E S
      input: ABCDEF
      output: SDKZQX
      ```

 3. 代码位于attack.py中

    从已知明文中可以找出：
    $S(A)=P_7^{-1}P_5S(A)$

    $S(T)=P_{14}^{-1}P_{13}S(T)$​ 

    $S(E)=P_{12}^{-1}P_{17}^{-1}P_0S(E)$

    在enigma中加入三个check方法

    ```python
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
    ```

    在attack.py中，遍历rotor排序方式和initial_position进行暴力搜索，在每种enigma初始化设置下尝试枚举A,T,E的插线板设置，并代入已知圈中进行检查。如果可以成功对应全部圈，则再进行一次已知明文加密，检查结果以筛选正确设置。

    

## 二、完善保密加密

2. 根据提示对密文进行按位异或，但考虑密文长度不一，故逐字进行异或。

   可以用space和字母做xor操作，对字母进行大小写切换，而两个字母做xor操作，结果将不在字母范围内。

   将两个密文做xor操作相当于将两个密文对应的明文做xor操作，如果结果中某个位置出现字母，则说明这两个明文的其中一个在该位置可能为空格。

   最终得出明文：

   > The secret message is: when using a stream cipher, never use the key more than once 