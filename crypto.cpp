#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

// Remove leading zeros
string clean(string s)
{
    int i = 0;

    while (i + 1 < s.size() && s[i] == '0')
        i++;

    return s.substr(i);
}

// --------------------------------------------------
// Compare two positive numbers
// returns 1 if a>b, 0 if equal, -1 if a<b
// --------------------------------------------------
int compare(string a, string b)
{
    a = clean(a);
    b = clean(b);

    if (a.size() != b.size())
        return a.size() > b.size() ? 1 : -1;

    if (a == b)
        return 0;

    return a > b ? 1 : -1;
}

// --------------------------------------------------
// Positive addition
//
// Example:
// 456 + 78
//
// 456
// +078
// ----
// 534
// --------------------------------------------------
string add(string a, string b)
{
    int i = a.size() - 1;
    int j = b.size() - 1;
    int carry = 0;

    string ans = "";

    while (i >= 0 || j >= 0 || carry)
    {
        int x = (i >= 0) ? a[i] - '0' : 0;
        int y = (j >= 0) ? b[j] - '0' : 0;

        int sum = x + y + carry;

        ans += char('0' + sum % 10);
        carry = sum / 10;

        i--;
        j--;
    }

    reverse(ans.begin(), ans.end());

    return clean(ans);
}

// --------------------------------------------------
// Positive subtraction
// Assumption: a >= b
//
// Example:
// 123 - 58
//
// 123
// -58
// ---
// 65
// --------------------------------------------------
string sub(string a, string b)
{
    int i = a.size() - 1;
    int j = b.size() - 1;
    int borrow = 0;

    string ans = "";

    while (i >= 0)
    {
        int x = a[i] - '0' - borrow;
        int y = (j >= 0) ? b[j] - '0' : 0;

        if (x < y)
        {
            x += 10;
            borrow = 1;
        }
        else
        {
            borrow = 0;
        }

        ans += char('0' + x - y);

        i--;
        j--;
    }

    reverse(ans.begin(), ans.end());

    return clean(ans);
}

// --------------------------------------------------
// Division by 2
// returns quotient and remainder
// --------------------------------------------------
pair<string, int> div2(string a)
{
    string q = "";
    int carry = 0;

    for (char c : a)
    {
        int x = carry * 10 + (c - '0');

        q += char('0' + x / 2);

        carry = x % 2;
    }

    return {clean(q), carry};
}

// --------------------------------------------------
// Positive multiplication
//
// Example:
// 13 * 6
//
// 6 is even  -> current = 26
// 3 is odd   -> answer = 26
// 1 is odd   -> answer = 78
//
// Result = 78
// --------------------------------------------------
string multiply(string a, string b, bool show = false)
{
    string ans = "0";
    string current = clean(a);
    string multiplier = clean(b);

    while (multiplier != "0")
    {
        auto p = div2(multiplier);

        if (show)
        {
            cout << "Current = " << current
                 << ", Multiplier = " << multiplier;
        }

        if (p.second == 1)
        {
            ans = add(ans, current);

            if (show)
                cout << " -> add current, Answer = " << ans;
        }

        cout << (show ? "\n" : "");

        current = add(current, current);
        multiplier = p.first;
    }

    return clean(ans);
}

// --------------------------------------------------
// Positive division
//
// Example:
// 157 / 12
//
// 15 -> 1, remainder 3
// 37 -> 3, remainder 1
//
// quotient = 13
// remainder = 1
// --------------------------------------------------
pair<string, string> dividePositive(
    string a,
    string b,
    bool show = false)
{
    string quotient = "";
    string remainder = "0";

    for (char c : a)
    {
        if (remainder == "0")
            remainder = string(1, c);
        else
            remainder += c;

        remainder = clean(remainder);

        int q = 0;

        while (compare(remainder, b) >= 0)
        {
            remainder = sub(remainder, b);
            q++;
        }

        quotient += char('0' + q);

        if (show)
        {
            cout << "Digit " << c
                 << " -> quotient digit = " << q
                 << ", remainder = " << remainder << "\n";
        }
    }

    return {clean(quotient), clean(remainder)};
}

// --------------------------------------------------
// GCD
//
// Example:
// 48 = 18*2 + 12
// 18 = 12*1 + 6
// 12 = 6*2 + 0
//
// GCD = 6
// --------------------------------------------------
string gcd(string a, string b, bool show = false)
{
    a = clean(a);
    b = clean(b);

    while (b != "0")
    {
        auto d = dividePositive(a, b);

        if (show)
        {
            cout << a << " = "
                 << b << " * "
                 << d.first << " + "
                 << d.second << "\n";
        }

        a = b;
        b = d.second;
    }

    return a;
}

// --------------------------------------------------
// Signed number
// --------------------------------------------------
struct Big
{
    string num;
    bool neg;
};

Big makeBig(string s)
{
    bool neg = false;

    if (s[0] == '-')
    {
        neg = true;
        s = s.substr(1);
    }

    s = clean(s);

    if (s == "0")
        neg = false;

    return {s, neg};
}

string printBig(Big a)
{
    if (a.neg && a.num != "0")
        return "-" + a.num;

    return a.num;
}

// --------------------------------------------------
// Signed addition
// --------------------------------------------------
Big addBig(Big a, Big b)
{
    if (a.neg == b.neg)
    {
        return makeBig(
            (a.neg ? "-" : "") +
            add(a.num, b.num)
        );
    }

    int c = compare(a.num, b.num);

    if (c == 0)
        return makeBig("0");

    if (c > 0)
    {
        return makeBig(
            (a.neg ? "-" : "") +
            sub(a.num, b.num)
        );
    }

    return makeBig(
        (b.neg ? "-" : "") +
        sub(b.num, a.num)
    );
}

Big negateBig(Big a)
{
    if (a.num != "0")
        a.neg = !a.neg;

    return a;
}

Big subBig(Big a, Big b)
{
    return addBig(a, negateBig(b));
}

// --------------------------------------------------
// Signed multiplication
// --------------------------------------------------
Big multiplyBig(Big a, Big b, bool show = false)
{
    string result = multiply(a.num, b.num, show);

    bool neg = a.neg ^ b.neg;

    if (result == "0")
        neg = false;

    return makeBig(
        (neg ? "-" : "") + result
    );
}

// --------------------------------------------------
// Signed division
// --------------------------------------------------
pair<Big, Big> divideBig(Big a, Big b, bool show = false)
{
    if (b.num == "0")
        throw runtime_error("Division by zero");

    auto d = dividePositive(a.num, b.num, show);

    bool qneg = a.neg ^ b.neg;
    bool rneg = a.neg;

    if (d.first == "0")
        qneg = false;

    if (d.second == "0")
        rneg = false;

    return {
        makeBig((qneg ? "-" : "") + d.first),
        makeBig((rneg ? "-" : "") + d.second)
    };
}

// --------------------------------------------------
// Modulo
// Result is between 0 and m-1
// --------------------------------------------------
Big modBig(Big a, Big m)
{
    if (m.num == "0")
        throw runtime_error("Modulo by zero");

    m.neg = false;

    auto d = divideBig(a, m);

    Big r = d.second;

    if (r.neg)
    {
        r.neg = false;
        r = subBig(m, r);
    }

    return r;
}

// --------------------------------------------------
// Extended GCD
//
// Finds:
// ax + by = gcd(a,b)
//
// Example:
// 56 = 15*3 + 11
// 15 = 11*1 + 4
// 11 = 4*2 + 3
// 4  = 3*1 + 1
//
// Result:
// x = -4
// y = 15
// gcd = 1
// --------------------------------------------------
struct Extended
{
    string g;
    Big x;
    Big y;
};

Extended extendedGCD(Big A, Big B, bool show = false)
{
    string r0 = A.num;
    string r1 = B.num;

    Big x0 = makeBig("1");
    Big x1 = makeBig("0");

    Big y0 = makeBig("0");
    Big y1 = makeBig("1");

    while (r1 != "0")
    {
        auto d = dividePositive(r0, r1);

        string q = d.first;

        if (show)
        {
            cout << r0 << " = "
                 << r1 << " * "
                 << q << " + "
                 << d.second << "\n";
        }

        Big qx = multiplyBig(x1, makeBig(q));
        Big qy = multiplyBig(y1, makeBig(q));

        Big nx = subBig(x0, qx);
        Big ny = subBig(y0, qy);

        r0 = r1;
        r1 = d.second;

        x0 = x1;
        x1 = nx;

        y0 = y1;
        y1 = ny;
    }

    if (A.neg)
        x0 = negateBig(x0);

    if (B.neg)
        y0 = negateBig(y0);

    return {r0, x0, y0};
}

// --------------------------------------------------
// Modular addition
// --------------------------------------------------
Big modAdd(Big a, Big b, Big m, bool show = false)
{
    Big x = modBig(a, m);
    Big y = modBig(b, m);

    if (show)
    {
        cout << a.num << " mod " << m.num
             << " = " << printBig(x) << "\n";

        cout << b.num << " mod " << m.num
             << " = " << printBig(y) << "\n";
    }

    return modBig(addBig(x, y), m);
}

// --------------------------------------------------
// Modular multiplication
// --------------------------------------------------
Big modMultiply(
    Big a,
    Big b,
    Big m,
    bool show = false)
{
    Big current = modBig(a, m);
    string multiplier = b.num;

    if (b.neg)
        current = negateBig(current);

    Big ans = makeBig("0");

    while (multiplier != "0")
    {
        auto p = div2(multiplier);

        if (show)
        {
            cout << "Current = "
                 << printBig(current)
                 << ", Multiplier = "
                 << multiplier << "\n";
        }

        if (p.second == 1)
        {
            ans = modAdd(ans, current, m);

            if (show)
                cout << "Add -> Answer = "
                     << printBig(ans) << "\n";
        }

        current = modAdd(current, current, m);
        multiplier = p.first;
    }

    return ans;
}

// --------------------------------------------------
// Modular exponentiation
//
// Example:
// 3^13 mod 7
//
// 13 odd -> answer = 3
// square 3 -> 2
// 6 even
// square 2 -> 4
// 3 odd -> answer = 5
// square 4 -> 2
// 1 odd -> answer = 3
//
// Result = 3
// --------------------------------------------------
Big modPower(
    Big base,
    Big exponent,
    Big m,
    bool show = false)
{
    if (exponent.neg)
        throw runtime_error("Exponent must be non-negative");

    Big ans = modBig(makeBig("1"), m);
    Big current = modBig(base, m);

    string e = exponent.num;

    while (e != "0")
    {
        auto p = div2(e);

        if (show)
        {
            cout << "Exponent = " << e
                 << ", Base = " << printBig(current)
                 << ", Answer = " << printBig(ans)
                 << "\n";
        }

        if (p.second == 1)
        {
            ans = modMultiply(ans, current, m);

            if (show)
                cout << "Multiply -> Answer = "
                     << printBig(ans) << "\n";
        }

        current = modMultiply(current, current, m);

        e = p.first;
    }

    return ans;
}

// --------------------------------------------------
// Modular inverse
// --------------------------------------------------
Big modInverse(Big a, Big m, bool show = false)
{
    Extended e = extendedGCD(a, m, show);

    if (e.g != "1")
        throw runtime_error(
            "Modular inverse does not exist"
        );

    return modBig(e.x, m);
}


// ==================================================
// MAIN
// ==================================================

int main()
{
    while (true)
    {
        cout << "\n==============================\n";
        cout << "BIG INTEGER CALCULATOR\n";
        cout << "==============================\n";

        cout << "1. GCD\n";
        cout << "2. Extended GCD\n";
        cout << "3. Modular Inverse\n";
        cout << "4. Modular Addition\n";
        cout << "5. Modular Multiplication\n";
        cout << "6. Modular Exponentiation\n";
        cout << "0. Exit\n";

        int choice;
        cout << "\nEnter choice: ";
        cin >> choice;

        if (choice == 0)
            break;

        string a, b, m;
        char show;

        try
        {
            if (choice == 1)
            {
                cout << "Enter a: ";
                cin >> a;

                cout << "Enter b: ";
                cin >> b;

                cout << "Show steps? (y/n): ";
                cin >> show;

                Big A = makeBig(a);
                Big B = makeBig(b);

                if (A.num == "0" && B.num == "0")
                    throw runtime_error(
                        "GCD(0,0) is undefined"
                    );

                cout << "\nGCD = "
                     << gcd(
                            A,
                            B,
                            show == 'y' || show == 'Y'
                        )
                     << "\n";
            }

            else if (choice == 2)
            {
                cout << "Enter a: ";
                cin >> a;

                cout << "Enter b: ";
                cin >> b;

                cout << "Show steps? (y/n): ";
                cin >> show;

                Big A = makeBig(a);
                Big B = makeBig(b);

                if (A.num == "0" && B.num == "0")
                    throw runtime_error(
                        "Extended GCD(0,0) is undefined"
                    );

                Extended e =
                    extendedGCD(
                        A,
                        B,
                        show == 'y' || show == 'Y'
                    );

                cout << "\nGCD = " << e.g << "\n";
                cout << "x = " << printBig(e.x) << "\n";
                cout << "y = " << printBig(e.y) << "\n";
            }

            else if (choice == 3)
            {
                cout << "Enter a: ";
                cin >> a;

                cout << "Enter modulus: ";
                cin >> m;

                cout << "Show steps? (y/n): ";
                cin >> show;

                Big ans =
                    modInverse(
                        makeBig(a),
                        makeBig(m),
                        show == 'y' || show == 'Y'
                    );

                cout << "\nInverse = "
                     << printBig(ans)
                     << "\n";
            }

            else if (choice == 4)
            {
                cout << "Enter a: ";
                cin >> a;

                cout << "Enter b: ";
                cin >> b;

                cout << "Enter modulus: ";
                cin >> m;

                cout << "Show steps? (y/n): ";
                cin >> show;

                Big ans =
                    modAdd(
                        makeBig(a),
                        makeBig(b),
                        makeBig(m),
                        show == 'y' || show == 'Y'
                    );

                cout << "\nAnswer = "
                     << printBig(ans)
                     << "\n";
            }

            else if (choice == 5)
            {
                cout << "Enter a: ";
                cin >> a;

                cout << "Enter b: ";
                cin >> b;

                cout << "Enter modulus: ";
                cin >> m;

                cout << "Show steps? (y/n): ";
                cin >> show;

                Big ans =
                    modMultiply(
                        makeBig(a),
                        makeBig(b),
                        makeBig(m),
                        show == 'y' || show == 'Y'
                    );

                cout << "\nAnswer = "
                     << printBig(ans)
                     << "\n";
            }

            else if (choice == 6)
            {
                cout << "Enter base: ";
                cin >> a;

                cout << "Enter exponent: ";
                cin >> b;

                cout << "Enter modulus: ";
                cin >> m;

                cout << "Show steps? (y/n): ";
                cin >> show;

                Big ans =
                    modPower(
                        makeBig(a),
                        makeBig(b),
                        makeBig(m),
                        show == 'y' || show == 'Y'
                    );

                cout << "\nAnswer = "
                     << printBig(ans)
                     << "\n";
            }

            else
            {
                cout << "Invalid choice.\n";
            }
        }
        catch (exception &e)
        {
            cout << "\nError: "
                 << e.what() << "\n";
        }
    }

    return 0;
}
