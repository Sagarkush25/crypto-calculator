#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

// Remove leading zeros
string clean(string a)
{
    int i = 0;

    while (i < (int)a.size() - 1 && a[i] == '0')
        i++;

    return a.substr(i);
}

// Compare two positive numbers
// 1  -> a > b
// 0  -> a == b
// -1 -> a < b
int cmp(string a, string b)
{
    a = clean(a);
    b = clean(b);

    if (a.size() > b.size())
        return 1;

    if (a.size() < b.size())
        return -1;

    if (a > b)
        return 1;

    if (a < b)
        return -1;

    return 0;
}

// --------------------------------------------------
// ADDITION
// --------------------------------------------------

string add(string a, string b)
{
    string ans = "";

    int i = (int)a.size() - 1;
    int j = (int)b.size() - 1;

    int carry = 0;

    while (i >= 0 || j >= 0 || carry)
    {
        int x = 0;
        int y = 0;

        if (i >= 0)
            x = a[i--] - '0';

        if (j >= 0)
            y = b[j--] - '0';

        int sum = x + y + carry;

        ans += char('0' + sum % 10);

        carry = sum / 10;
    }

    reverse(ans.begin(), ans.end());

    return clean(ans);
}

// --------------------------------------------------
// SUBTRACTION
// Assumption: a >= b
// --------------------------------------------------

string sub(string a, string b)
{
    string ans = "";

    int i = (int)a.size() - 1;
    int j = (int)b.size() - 1;

    int borrow = 0;

    while (i >= 0)
    {
        int x = (a[i] - '0') - borrow;
        int y = 0;

        if (j >= 0)
            y = b[j--] - '0';

        if (x < y)
        {
            x += 10;
            borrow = 1;
        }
        else
        {
            borrow = 0;
        }

        ans += char('0' + (x - y));

        i--;
    }

    reverse(ans.begin(), ans.end());

    return clean(ans);
}

// --------------------------------------------------
// MULTIPLICATION
// --------------------------------------------------

string multiply(string a, string b)
{
    string ans = "0";

    // current = a, then a*2, a*4, a*8...
    string current = a;

    // multiplier = b, then b/2, b/4...
    string multiplier = b;

    while (multiplier != "0")
    {
        // Check if multiplier is odd
        char last = multiplier.back();

        bool odd =
            last == '1' ||
            last == '3' ||
            last == '5' ||
            last == '7' ||
            last == '9';

        // If odd, add current to answer
        if (odd)
            ans = add(ans, current);

        // current = current * 2
        current = add(current, current);

        // multiplier = multiplier / 2
        string quotient = "";
        int carry = 0;

        for (char c : multiplier)
        {
            int num = carry * 10 + (c - '0');

            quotient += char('0' + num / 2);

            carry = num % 2;
        }

        multiplier = clean(quotient);
    }

    return clean(ans);
}

// --------------------------------------------------
// DIVISION
// Returns:
// first  = quotient
// second = remainder
// --------------------------------------------------

pair<string, string> divide(string a, string b)
{
    a = clean(a);
    b = clean(b);

    string quotient = "";
    string remainder = "0";

    for (char digit : a)
    {
        // Bring down next digit
        if (remainder == "0")
            remainder = string(1, digit);
        else
            remainder += digit;

        remainder = clean(remainder);

        // Find one quotient digit
        int count = 0;
        string temp = "0";

        while (count < 10)
        {
            string next = add(temp, b);

            if (cmp(next, remainder) > 0)
                break;

            temp = next;
            count++;
        }

        quotient += char('0' + count);

        // remainder = remainder - temp
        remainder = sub(remainder, temp);
    }

    return {
        clean(quotient),
        clean(remainder)
    };
}

// --------------------------------------------------
// EUCLIDEAN GCD
// --------------------------------------------------

string gcd(string a, string b)
{
    while (b != "0")
    {
        string r = divide(a, b).second;

        a = b;
        b = r;
    }

    return a;
}

// --------------------------------------------------
// SIGNED NUMBER
// --------------------------------------------------

struct Signed
{
    string n;
    bool negative;
};

// Change sign
Signed negateSigned(Signed a)
{
    if (a.n != "0")
        a.negative = !a.negative;

    return a;
}

// Add signed numbers
Signed addSigned(Signed a, Signed b)
{
    // Same signs
    if (a.negative == b.negative)
    {
        return {
            add(a.n, b.n),
            a.negative
        };
    }

    // Different signs
    if (cmp(a.n, b.n) >= 0)
    {
        return {
            sub(a.n, b.n),
            a.negative
        };
    }

    return {
        sub(b.n, a.n),
        b.negative
    };
}

// Subtract signed numbers
// a - b = a + (-b)
Signed subSigned(Signed a, Signed b)
{
    return addSigned(a, negateSigned(b));
}

// Multiply signed number by positive number
Signed multiplySigned(Signed a, string b)
{
    string ans = "0";
    string current = a.n;
    string multiplier = b;

    while (multiplier != "0")
    {
        char last = multiplier.back();

        bool odd =
            last == '1' ||
            last == '3' ||
            last == '5' ||
            last == '7' ||
            last == '9';

        if (odd)
            ans = add(ans, current);

        current = add(current, current);

        // Divide multiplier by 2
        string quotient = "";
        int carry = 0;

        for (char c : multiplier)
        {
            int num = carry * 10 + (c - '0');

            quotient += char('0' + num / 2);

            carry = num % 2;
        }

        multiplier = clean(quotient);
    }

    return {
        ans,
        a.negative
    };
}

// --------------------------------------------------
// EXTENDED EUCLIDEAN GCD
// --------------------------------------------------

struct Extended
{
    string gcd;
    Signed x;
    Signed y;
};

Extended extendedGCD(string a, string b)
{
    // Remainders
    string r1 = a;
    string r2 = b;

    // x coefficients
    Signed x1 = {"1", false};
    Signed x2 = {"0", false};

    // y coefficients
    Signed y1 = {"0", false};
    Signed y2 = {"1", false};

    while (r2 != "0")
    {
        // r1 = q*r2 + remainder
        pair<string, string> d = divide(r1, r2);

        string q = d.first;
        string remainder = d.second;

        // new x = x1 - q*x2
        Signed nx =
            subSigned(
                x1,
                multiplySigned(x2, q)
            );

        // new y = y1 - q*y2
        Signed ny =
            subSigned(
                y1,
                multiplySigned(y2, q)
            );

        // Shift remainders
        r1 = r2;
        r2 = remainder;

        // Shift x
        x1 = x2;
        x2 = nx;

        // Shift y
        y1 = y2;
        y2 = ny;
    }

    return {
        r1,
        x1,
        y1
    };
}

// --------------------------------------------------
// PRINT SIGNED NUMBER
// --------------------------------------------------

void printSigned(Signed a)
{
    if (a.negative && a.n != "0")
        cout << "-";

    cout << a.n;
}

// --------------------------------------------------
// EXECUTE OPERATION HELPER
// --------------------------------------------------

void runOperation(int choice, const string& a, const string& b)
{
    // Addition
    if (choice == 1)
    {
        cout << "Answer = " << add(a, b) << "\n";
    }
    // Subtraction
    else if (choice == 2)
    {
        if (cmp(a, b) < 0)
        {
            cout << "For this program, a must be >= b.\n";
        }
        else
        {
            cout << "Answer = " << sub(a, b) << "\n";
        }
    }
    // Multiplication
    else if (choice == 3)
    {
        cout << "Answer = " << multiply(a, b) << "\n";
    }
    // Division
    else if (choice == 4)
    {
        if (clean(b) == "0")
        {
            cout << "Division by zero is not allowed.\n";
        }
        else
        {
            pair<string, string> ans = divide(a, b);
            cout << "Quotient = " << ans.first << "\n";
            cout << "Remainder = " << ans.second << "\n";
        }
    }
    // GCD
    else if (choice == 5)
    {
        cout << "GCD = " << gcd(a, b) << "\n";
    }
    // Extended GCD
    else if (choice == 6)
    {
        Extended ans = extendedGCD(a, b);
        cout << "GCD = " << ans.gcd << "\n";
        cout << "x = ";
        printSigned(ans.x);
        cout << "\ny = ";
        printSigned(ans.y);
        cout << "\n";
    }
    else
    {
        cout << "Invalid choice.\n";
    }
}

// Map textual operation name to numeric choice
int parseOperationChoice(const string& op)
{
    if (op == "1" || op == "add" || op == "addition") return 1;
    if (op == "2" || op == "sub" || op == "subtraction") return 2;
    if (op == "3" || op == "mul" || op == "multiply" || op == "multiplication") return 3;
    if (op == "4" || op == "div" || op == "divide" || op == "division") return 4;
    if (op == "5" || op == "gcd") return 5;
    if (op == "6" || op == "extgcd" || op == "extended_gcd") return 6;
    return -1;
}

// --------------------------------------------------
// MAIN
// --------------------------------------------------

int main(int argc, char* argv[])
{
    // If command-line arguments are passed: ./crypto <op> <a> <b>
    if (argc >= 4)
    {
        int choice = parseOperationChoice(argv[1]);
        if (choice == -1)
        {
            cout << "Invalid operation: " << argv[1] << "\n";
            cout << "Supported operations: add, sub, mul, div, gcd, extgcd (or numbers 1 to 6)\n";
            return 1;
        }

        string a = argv[2];
        string b = argv[3];
        runOperation(choice, a, b);
        return 0;
    }

    // Default: Interactive menu mode
    while (true)
    {
        cout << "\n";
        cout << "========== BIG INTEGER CALCULATOR ==========\n";
        cout << "1. Addition\n";
        cout << "2. Subtraction\n";
        cout << "3. Multiplication\n";
        cout << "4. Division\n";
        cout << "5. Euclidean GCD\n";
        cout << "6. Extended Euclidean GCD\n";
        cout << "0. Exit\n";

        cout << "Enter choice: ";

        int choice;
        if (!(cin >> choice))
            break;

        if (choice == 0)
            break;

        string a, b;
        cout << "Enter a: ";
        cin >> a;

        cout << "Enter b: ";
        cin >> b;

        runOperation(choice, a, b);
    }

    return 0;
}
