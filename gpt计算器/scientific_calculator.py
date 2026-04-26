import math
import os
import re
import sys


def _prepare_tkinter_runtime():
    """Help frozen apps find Tcl/Tk data files before importing tkinter."""
    base_dir = getattr(sys, "_MEIPASS", None)
    if base_dir is None and getattr(sys, "frozen", False):
        base_dir = os.path.join(os.path.dirname(sys.executable), "_internal")
    if base_dir is None:
        return

    tcl_dir = os.path.join(base_dir, "_tcl_data")
    tk_dir = os.path.join(base_dir, "_tk_data")
    if os.path.isdir(tcl_dir):
        os.environ.setdefault("TCL_LIBRARY", tcl_dir)
    if os.path.isdir(tk_dir):
        os.environ.setdefault("TK_LIBRARY", tk_dir)


def _show_startup_error(message):
    try:
        import ctypes

        ctypes.windll.user32.MessageBoxW(None, message, "科学计算器", 0x10)
    except Exception:
        print(message, file=sys.stderr)


_prepare_tkinter_runtime()

try:
    import tkinter as tk
    from tkinter import messagebox
except ModuleNotFoundError as exc:
    _show_startup_error(
        "程序缺少 tkinter 运行组件，无法启动图形界面。\n\n"
        "请运行 dist\\ScientificCalculator\\ScientificCalculator.exe，"
        "并保留旁边的 _internal 文件夹。\n\n"
        f"详细错误：{exc}"
    )
    raise SystemExit(1) from exc


class ScientificCalculator(tk.Tk):
    AUTHOR = "JOEL"

    def __init__(self):
        super().__init__()
        self.title(f"科学计算器 - {self.AUTHOR}")
        self.resizable(False, False)
        self.configure(padx=10, pady=10, bg="#202124")

        self.ans = 0
        self.expression = tk.StringVar()
        self.angle_mode = tk.StringVar(value="DEG")

        self._build_display()
        self._build_buttons()

    def _build_display(self):
        mode_frame = tk.Frame(self, bg="#202124")
        mode_frame.grid(row=0, column=0, columnspan=6, sticky="ew", pady=(0, 6))

        tk.Radiobutton(
            mode_frame,
            text="角度 DEG",
            variable=self.angle_mode,
            value="DEG",
            bg="#202124",
            fg="#f1f3f4",
            selectcolor="#3c4043",
            activebackground="#202124",
            activeforeground="#f1f3f4",
        ).pack(side="left")

        tk.Radiobutton(
            mode_frame,
            text="弧度 RAD",
            variable=self.angle_mode,
            value="RAD",
            bg="#202124",
            fg="#f1f3f4",
            selectcolor="#3c4043",
            activebackground="#202124",
            activeforeground="#f1f3f4",
        ).pack(side="left")

        tk.Label(
            mode_frame,
            text=f"作者：{self.AUTHOR}",
            bg="#202124",
            fg="#bdc1c6",
            font=("Microsoft YaHei UI", 9),
        ).pack(side="right")

        entry = tk.Entry(
            self,
            textvariable=self.expression,
            font=("Consolas", 22),
            justify="right",
            bd=0,
            width=26,
            bg="#303134",
            fg="#f1f3f4",
            insertbackground="#f1f3f4",
        )
        entry.grid(row=1, column=0, columnspan=6, sticky="ew", pady=(0, 10), ipady=12)
        entry.focus_set()
        entry.bind("<Return>", lambda _event: self.calculate())
        entry.bind("<Escape>", lambda _event: self.clear())

    def _build_buttons(self):
        buttons = [
            ("sin", "sin(", 2, 0), ("cos", "cos(", 2, 1), ("tan", "tan(", 2, 2),
            ("log", "log10(", 2, 3), ("ln", "log(", 2, 4), ("清空", None, 2, 5),
            ("asin", "asin(", 3, 0), ("acos", "acos(", 3, 1), ("atan", "atan(", 3, 2),
            ("sqrt", "sqrt(", 3, 3), ("x^2", "**2", 3, 4), ("退格", None, 3, 5),
            ("pi", "pi", 4, 0), ("e", "e", 4, 1), ("(", "(", 4, 2),
            (")", ")", 4, 3), ("^", "**", 4, 4), ("/", "/", 4, 5),
            ("7", "7", 5, 0), ("8", "8", 5, 1), ("9", "9", 5, 2),
            ("*", "*", 5, 3), ("%", "%", 5, 4), ("!", "!", 5, 5),
            ("4", "4", 6, 0), ("5", "5", 6, 1), ("6", "6", 6, 2),
            ("-", "-", 6, 3), ("1/x", "1/(", 6, 4), ("abs", "abs(", 6, 5),
            ("1", "1", 7, 0), ("2", "2", 7, 1), ("3", "3", 7, 2),
            ("+", "+", 7, 3), ("exp", "exp(", 7, 4), ("=", None, 7, 5),
            ("0", "0", 8, 0), ("00", "00", 8, 1), (".", ".", 8, 2),
            ("Ans", "ans", 8, 3), (",", ",", 8, 4), ("pow", "pow(", 8, 5),
        ]

        for text, value, row, column in buttons:
            button = tk.Button(
                self,
                text=text,
                command=self._button_command(text, value),
                width=7,
                height=2,
                bd=0,
                font=("Microsoft YaHei UI", 11),
                bg=self._button_color(text),
                fg="#f1f3f4",
                activebackground="#5f6368",
                activeforeground="#ffffff",
            )
            button.grid(row=row, column=column, padx=3, pady=3, sticky="nsew")

        for column in range(6):
            self.grid_columnconfigure(column, weight=1)

    def _button_command(self, text, value):
        if text == "清空":
            return self.clear
        if text == "退格":
            return self.delete_last
        if text == "=":
            return self.calculate
        return lambda: self.add_text(value)

    @staticmethod
    def _button_color(text):
        if text == "=":
            return "#1a73e8"
        if text in {"清空", "退格"}:
            return "#b3261e"
        if text in {"+", "-", "*", "/", "%", "^"}:
            return "#5f6368"
        return "#3c4043"

    def add_text(self, text):
        self.expression.set(self.expression.get() + text)

    def clear(self):
        self.expression.set("")

    def delete_last(self):
        self.expression.set(self.expression.get()[:-1])

    def calculate(self):
        expr = self.expression.get().strip()
        if not expr:
            return

        try:
            result = self._safe_eval(expr)
            result_text = self._format_result(result)
        except Exception as exc:
            messagebox.showerror("计算错误", self._format_error_message(exc))
            return

        self.expression.set(result_text)
        self.ans = result

    def _safe_eval(self, expr):
        def to_radians(value):
            return math.radians(value) if self.angle_mode.get() == "DEG" else value

        def from_radians(value):
            return math.degrees(value) if self.angle_mode.get() == "DEG" else value

        allowed_names = {
            "abs": abs,
            "ans": self.ans,
            "e": math.e,
            "exp": math.exp,
            "factorial": self._factorial,
            "log": math.log,
            "log10": math.log10,
            "pi": math.pi,
            "pow": pow,
            "sqrt": math.sqrt,
            "sin": lambda x: math.sin(to_radians(x)),
            "cos": lambda x: math.cos(to_radians(x)),
            "tan": lambda x: math.tan(to_radians(x)),
            "asin": lambda x: from_radians(math.asin(x)),
            "acos": lambda x: from_radians(math.acos(x)),
            "atan": lambda x: from_radians(math.atan(x)),
        }

        normalized_expr = self._normalize_expression(expr)
        return eval(normalized_expr, {"__builtins__": {}}, allowed_names)

    @classmethod
    def _normalize_expression(cls, expr):
        expr = (
            expr.replace(" ", "")
            .replace("，", ",")
            .replace("×", "*")
            .replace("÷", "/")
            .replace("^", "**")
        )
        tokens = cls._tokenize_expression(expr)
        tokens = cls._insert_implicit_multiplication(tokens)
        tokens = cls._convert_factorials(tokens)
        tokens = cls._insert_implicit_multiplication(tokens)
        normalized = "".join(token for token, _kind in tokens)
        return cls._close_missing_parentheses(normalized)

    @staticmethod
    def _tokenize_expression(expr):
        token_pattern = re.compile(
            r"""
            (?P<number>(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?)
            |(?P<name>[A-Za-z_][A-Za-z_0-9]*)
            |(?P<operator>\*\*|[+\-*/%!])
            |(?P<lparen>\()
            |(?P<rparen>\))
            |(?P<comma>,)
            """,
            re.VERBOSE,
        )
        functions = {
            "abs",
            "acos",
            "asin",
            "atan",
            "cos",
            "exp",
            "factorial",
            "log",
            "log10",
            "pow",
            "sin",
            "sqrt",
            "tan",
        }
        constants = {"ans", "e", "pi"}
        aliases = {"ln": "log"}

        tokens = []
        pos = 0
        while pos < len(expr):
            match = token_pattern.match(expr, pos)
            if match is None:
                raise ValueError(f"不支持的字符：{expr[pos]}")

            value = match.group()
            kind = match.lastgroup
            if kind == "name":
                value = aliases.get(value.lower(), value)
                lowered = value.lower()
                if lowered in functions:
                    value, kind = lowered, "function"
                elif lowered in constants:
                    value, kind = lowered, "constant"
                else:
                    constant_number = re.fullmatch(r"(ans|pi|e)(\d+(?:\.\d*)?|\.\d+)", lowered)
                    if constant_number:
                        tokens.append((constant_number.group(1), "constant"))
                        tokens.append((constant_number.group(2), "number"))
                        pos = match.end()
                        continue
                    raise ValueError(f"未知的函数或变量：{value}")

            tokens.append((value, kind))
            pos = match.end()

        return tokens

    @staticmethod
    def _insert_implicit_multiplication(tokens):
        if not tokens:
            return tokens

        result = [tokens[0]]
        left_kinds = {"number", "constant", "rparen"}
        right_kinds = {"number", "constant", "function", "lparen"}

        for token, kind in tokens[1:]:
            previous_token, previous_kind = result[-1]
            should_multiply = previous_kind in left_kinds and kind in right_kinds
            if previous_kind == "function" and kind == "lparen":
                should_multiply = False
            if previous_token == "**" or token == "**":
                should_multiply = False
            if should_multiply:
                result.append(("*", "operator"))
            result.append((token, kind))

        return result

    @staticmethod
    def _convert_factorials(tokens):
        result = []
        for token, kind in tokens:
            if token != "!":
                result.append((token, kind))
                continue

            if not result:
                raise ValueError("阶乘前面需要有数字或表达式。")

            _previous_token, previous_kind = result[-1]
            if previous_kind in {"number", "constant"}:
                operand = result.pop()
                result.extend([
                    ("factorial", "function"),
                    ("(", "lparen"),
                    operand,
                    (")", "rparen"),
                ])
                continue

            if previous_kind == "rparen":
                depth = 0
                start = None
                for index in range(len(result) - 1, -1, -1):
                    current_token, _current_kind = result[index]
                    if current_token == ")":
                        depth += 1
                    elif current_token == "(":
                        depth -= 1
                        if depth == 0:
                            start = index
                            break
                if start is None:
                    raise ValueError("阶乘表达式的括号不完整。")
                group = result[start:]
                result[start:] = [
                    ("factorial", "function"),
                    ("(", "lparen"),
                    *group,
                    (")", "rparen"),
                ]
                continue

            raise ValueError("阶乘只能用于数字或括号表达式后面。")

        return result

    @staticmethod
    def _close_missing_parentheses(expr):
        balance = 0
        for char in expr:
            if char == "(":
                balance += 1
            elif char == ")":
                balance -= 1
                if balance < 0:
                    raise ValueError("右括号过多，请检查括号是否匹配。")
        return expr + ")" * balance

    @staticmethod
    def _factorial(value):
        if int(value) != value or value < 0:
            raise ValueError("阶乘只能用于非负整数。")
        return math.factorial(int(value))

    @staticmethod
    def _format_result(result):
        if isinstance(result, float):
            if math.isnan(result) or math.isinf(result):
                raise ValueError("计算结果不是有效数字。")
            if result.is_integer():
                return str(int(result))
            return f"{result:.12g}"
        return str(result)

    @staticmethod
    def _format_error_message(exc):
        if isinstance(exc, ZeroDivisionError):
            return "除数不能为 0。"
        if isinstance(exc, OverflowError):
            return "计算结果过大，超出了程序可表示的范围。"
        if isinstance(exc, SyntaxError):
            return "表达式格式不正确，请检查运算符和括号。"

        text = str(exc)
        translations = {
            "math domain error": "函数输入超出允许范围，例如 sqrt 不能计算负数，log 不能计算非正数。",
            "invalid decimal literal": "数字格式不正确，请检查是否少写了运算符。",
            "unsupported operand type": "运算对象类型不正确，请检查表达式。",
        }
        for source, target in translations.items():
            if source in text:
                return target
        return f"表达式无法计算：{text}"


if __name__ == "__main__":
    app = ScientificCalculator()
    app.mainloop()
