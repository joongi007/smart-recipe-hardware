# 점화 콕 손잡이

#### 지원할 기능
- 현재 몇 단인지 인식.
- 블루투스 연결 지원.

#### 사용된 부품
|부품   |역할|규격(mm)|필수|수량|링크|
|---|---|---|---|---|---|
|hc-05|무선 통신|37 x 17 x 5|y|1|[구매](https://www.coupang.com/vp/products/248838944?itemId=787322013&isAddedCart=)|
|arduino uno|제어|69 x 53 x 12|y|1|[구매](https://www.coupang.com/vp/products/84959309?itemId=964056709&isAddedCart=)|
|FSR-16|버너 손잡이용|10 x 10 x 17.6|y|1|[구매](https://www.icbanq.com/P007628094)|
|저항 10K|풀다운|10 x 10 x 17.6|y|5|[구매](http://itempage3.auction.co.kr/DetailView.aspx?ItemNo=B691389744&frm3=V2)|

#### 회로도
![img1](ignition_cock_handle_schematic.png)

#### 핀 연결 정보
|선 색깔|아두이노|hc-05|FSR-16|
|----|---|---|---|
|빨강|5v|Vcc|C|
|검정|GND|GND|    |
|흰색|2|TXD|    |
|초록|3|RXD|    |
|노랑|7|    |8|
|회색|6|    |4|
|파랑|5|    |2|
|주황|4|    |1|