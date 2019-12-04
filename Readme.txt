******************************************************************************************
수정내역

ver.1.0.0
- 신규 braket에 따른 slope, offset 조정 (tilt 항목)
- 5point 기준 data 추출
- 2차 regression 적용되어 있음.
- 상부-하부 데이터 측정 후 평균 data 사용 필요.

ver.1.0.1
- slope 분모 변경(120000)
- Auto start 항목 중 Auto stop 제거

ver.1.0.2
- New Jig 맞춤 측정 point 개선(nmax data 기준)
- filtering > 5point median으로 변경
- 반드시 new jig 양측 끝에 point를 잡아야 정상 동작함.

ver.1.0.3
- 센서 die 기준 5point median 변경(버그 개선)
