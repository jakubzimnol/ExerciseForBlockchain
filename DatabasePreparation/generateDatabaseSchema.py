import mysql.connector
import sys
if sys.argv[1] == "--help" or sys.argv[1] == "-h":
    print("usage: generateDatabaseSchema <host> <user> <password> <database>")
    exit()
if len(sys.argv) < 5:
    print("Not enough parameters. Use --help to check usage")
    exit()
if len(sys.argv) > 5:
    print("Too many parameters. Use --help to check usage")
    exit()

mydb = mysql.connector.connect(
  host=sys.argv[1],
  user=sys.argv[2],
  password=sys.argv[3],
  database=sys.argv[4]
)

mycursor = mydb.cursor()

mycursor.execute("CREATE TABLE Users (id INT PRIMARY KEY AUTO_INCREMENT, email VARCHAR(255), "
                "nickname VARCHAR(255), password VARCHAR(255), isActive BOOLEAN)")
print ("Created table Users")

sql = "INSERT INTO Users (email, nickname, password, isActive) VALUES (%s, %s, %s, %s)"
val = [
    ('JohnDoe@example.com', 'JohnDoe', 'Password1234', True),
    ('InActiveUser@example.com', 'InactiveUser', 'Password4321', False)
]

mycursor.executemany(sql, val)

mydb.commit()

print(mycursor.rowcount, "record inserted.")