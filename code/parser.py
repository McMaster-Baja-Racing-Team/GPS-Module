# Python 3. Will get WRONG results with Python 2
# Parse GPGGA or GPRMC strings
#   Given GPGGA, returns: Time, Lat, Long, Quality, # Satellites, Altitude
#   Given GPRMA, returns: Date, Time, Lat, Long, Speed

# Parse a string of GGA or RMA format, returns parsed string
def parse_string(st):
    st = st.split(",")

    # $GPGGA: NMEA-Type, Time, Lat, Long, Fix Quality, # Satelites, HDOP, Alt, Geoid Height, time since last update, DGPS id, Checksum
    # Return: Time, Lat, Long, Quality, # Satellites, Altitude
    if st[0] == "$GPGGA":
        return to_time(st[1]) + ", " + dms_to_dd(st[2], st[3]) + ", " + dms_to_dd(st[4], st[5]) + ", " + st[6] + ", " + st[7] + ", " + st[9]

    # $GPRMC: NMEA-Type, Time [UTC], Receiver Warning [A = Ok], Lat, Long, Speed [Knots], Course, Date, Magnetic Variation, Checksum
    # Return: Date, Time, Lat, Long, Speed
    elif st[0] == "$GPRMC":
        return to_date(st[9]) + ", " + to_time(st[1]) + ", " + dms_to_dd(st[3], st[4]) + ", " + dms_to_dd(st[5], st[6]) + ", " + knots_to_km(st[7])

# ddmmyy to dd:mm:yy
def to_date(num):
    return num[0:2] + "/" + num[2:4] + "/" + num[4:6]

# hhmmss to hh:mm:ss [UTC]
def to_time(num):
    return num[0:2] + ":" + num[2:4] + ":" + num[4:6] + " UTC"

# Latitude/Longitude DMS to DD format
#   DegMinSec to Decimal Degrees
def dms_to_dd(num, dir):
    num = num.split(".")
    d = (float)(num[0][:-2])
    m = (float)(num[0][-2:])
    s = (float)(num[1])
    dd = d + m/60 + s/3600

    if dir == "S" or dir == "W":
        dd *= -1

    return str(dd)

# knots to km/h
def knots_to_km(knots):
    return str(float(knots)*1.85) + " km/h"

#print("GGA: hh:mm:ss [UTC], lat, long, Quality, # Satellites, Altitude [M abv. Sea lvl]")
#print("RMC: dd/mm/yy, hh:mm:ss [UTC], lat, long, km/h")
print(parse_string("$GPGGA,121620.422,5231.644,N,01325.770,E,1,12,1.0,0.0,M,0.0,M,,*6A"))
print(parse_string("$GPRMC,121620.422,A,5231.644,N,01325.770,E,2384.4,013.6,220418,000.0,W*4E"))
