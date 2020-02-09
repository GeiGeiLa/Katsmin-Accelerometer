# Katsmin Accelerometer

## Introduction to KatsAcc

This app allows users to monitor **daily amount of exercise**, especially for the elders, who need persistent exercise to prevent health problems.

:::info
This document is for the beta version of KatsAcc. **DO NOT** refer to the documentation for the alpha version, for deprecation of most features on it.
:::

## Sending/Receiving commands to/from ACC

### Timestamp

Steps to interpret receive timestamps:

1. **Reverse** order in **bytes**
2. Convert to **binary** digits
3. Split binary digits

Received **BYTES (2 digits in hexdecimal form)** should be **reversed**, for example:

```plain
Received hex pattern: 66 74 61 02
```

You should reverse orders in **BYTE**, i.e. interpret patterns to this form:

```plain
Interpretation in hex: 02 61 74 66
```

Converting to binary form and split them:

```plain
(binary)     00 0000 1001 10000 10111 010001 100110
```

The data of these digit patterns are in this order:

- num of bits ->field
- 2 -> don't care
- 4 -> years after 2019 (for example 0000 means 2019)
- 4 -> month
- 5 -> day
- 5 -> hour
- 6 -> minute
- 6 -> second

