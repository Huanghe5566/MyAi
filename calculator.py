import tkinter as tk
from tkinter import font
import math
import re


# ─── 主题颜色 ───────────────────────────────────────────────
BG_MAIN      = "#1C1C1E"
BG_DISPLAY   = "#2C2C2E"
BG_BTN_NUM   = "#3A3A3C"
BG_BTN_OP    = "#FF9F0A"
BG_BTN_FUNC  = "#2C2C2E"
BG_BTN_CLEAR = "#636366"
FG_WHITE     = "#FFFFFF"
FG_ORANGE    = "#FF9F0A"
FG_GRAY      = "#EBEBF5"
HOVER_NUM    = "#4A4A4C"
HOVER_OP     = "#FFB340"
HOVER_FUNC   = "#3C3C3E"
HOVER_CLEAR  = "#737376"


class ScientificCalculator(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("科学计算器")
        self.resizable(False, False)
        self.configure(bg=BG_MAIN)

        # 状态
        self.expression = ""      # 当前表达式（内部用）
        self.display_expr = ""    # 显示用表达式
        self.result = "0"
        self.just_calculated = False
        self.tokens = []          # [(expr_part, disp_part), ...]
        self.angle_mode = "DEG"   # DEG / RAD

        self._build_ui()
        self._bind_keyboard()

    # ── 构建界面 ────────────────────────────────────────────
    def _build_ui(self):
        # 字体
        self.font_display_small = font.Font(family="SF Pro Display", size=14, weight="normal")
        self.font_display_large = font.Font(family="SF Pro Display", size=38, weight="normal")
        self.font_btn_large     = font.Font(family="SF Pro Display", size=18)
        self.font_btn_small     = font.Font(family="SF Pro Display", size=13)
        self.font_mode          = font.Font(family="SF Pro Display", size=10, weight="bold")

        # 顶部角度/弧度切换
        mode_frame = tk.Frame(self, bg=BG_MAIN)
        mode_frame.pack(fill="x", padx=14, pady=(12, 0))

        self.mode_btn = tk.Label(
            mode_frame, text="DEG", fg=FG_ORANGE, bg=BG_MAIN,
            font=self.font_mode, cursor="hand2"
        )
        self.mode_btn.pack(side="left")
        self.mode_btn.bind("<Button-1>", lambda e: self._toggle_mode())

        # 显示区
        display_frame = tk.Frame(self, bg=BG_DISPLAY, bd=0)
        display_frame.pack(fill="x", padx=14, pady=8)

        self.lbl_history = tk.Label(
            display_frame, text="", anchor="e",
            fg="#8E8E93", bg=BG_DISPLAY,
            font=self.font_display_small,
            padx=16
        )
        self.lbl_history.pack(fill="x", pady=(8, 0))

        self.lbl_result = tk.Label(
            display_frame, text="0", anchor="e",
            fg=FG_WHITE, bg=BG_DISPLAY,
            font=self.font_display_large,
            padx=16
        )
        self.lbl_result.pack(fill="x", pady=(0, 10))

        # 按钮区
        btn_frame = tk.Frame(self, bg=BG_MAIN)
        btn_frame.pack(padx=14, pady=(0, 14))

        self._create_buttons(btn_frame)

    def _create_buttons(self, parent):
        # 布局定义: (显示文字, 内部标识, 背景色, 前景色, colspan)
        layout = [
            # 第1行 - 科学函数
            [("sin",  "sin",   BG_BTN_FUNC, FG_GRAY,   1),
             ("cos",  "cos",   BG_BTN_FUNC, FG_GRAY,   1),
             ("tan",  "tan",   BG_BTN_FUNC, FG_GRAY,   1),
             ("π",    "pi",    BG_BTN_FUNC, FG_ORANGE, 1),
             ("e",    "e",     BG_BTN_FUNC, FG_ORANGE, 1)],
            # 第2行
            [("xⁿ",  "pow",   BG_BTN_FUNC, FG_GRAY,   1),
             ("√",   "sqrt",  BG_BTN_FUNC, FG_GRAY,   1),
             ("log", "log10", BG_BTN_FUNC, FG_GRAY,   1),
             ("ln",  "log",   BG_BTN_FUNC, FG_GRAY,   1),
             ("1/x", "inv",   BG_BTN_FUNC, FG_GRAY,   1)],
            # 第3行
            [("(",   "(",     BG_BTN_FUNC, FG_GRAY,   1),
             (")",   ")",     BG_BTN_FUNC, FG_GRAY,   1),
             ("%",   "%",     BG_BTN_FUNC, FG_GRAY,   1),
             ("AC",  "AC",    BG_BTN_CLEAR,FG_WHITE,  1),
             ("⌫",   "DEL",   BG_BTN_CLEAR,FG_WHITE,  1)],
            # 第4行
            [("7",   "7",     BG_BTN_NUM,  FG_WHITE,  1),
             ("8",   "8",     BG_BTN_NUM,  FG_WHITE,  1),
             ("9",   "9",     BG_BTN_NUM,  FG_WHITE,  1),
             ("÷",   "/",     BG_BTN_OP,   FG_WHITE,  1),
             ("x!",  "fact",  BG_BTN_FUNC, FG_GRAY,   1)],
            # 第5行
            [("4",   "4",     BG_BTN_NUM,  FG_WHITE,  1),
             ("5",   "5",     BG_BTN_NUM,  FG_WHITE,  1),
             ("6",   "6",     BG_BTN_NUM,  FG_WHITE,  1),
             ("×",   "*",     BG_BTN_OP,   FG_WHITE,  1),
             ("x²",  "sq",    BG_BTN_FUNC, FG_GRAY,   1)],
            # 第6行
            [("1",   "1",     BG_BTN_NUM,  FG_WHITE,  1),
             ("2",   "2",     BG_BTN_NUM,  FG_WHITE,  1),
             ("3",   "3",     BG_BTN_NUM,  FG_WHITE,  1),
             ("−",   "-",     BG_BTN_OP,   FG_WHITE,  1),
             ("10ⁿ", "10pow", BG_BTN_FUNC, FG_GRAY,   1)],
            # 第7行
            [("0",   "0",     BG_BTN_NUM,  FG_WHITE,  2),
             (".",   ".",     BG_BTN_NUM,  FG_WHITE,  1),
             ("+",   "+",     BG_BTN_OP,   FG_WHITE,  1),
             ("=",   "=",     BG_BTN_OP,   FG_WHITE,  1)],
        ]

        btn_w = 62
        btn_h = 52
        gap   = 8

        for r, row in enumerate(layout):
            col_offset = 0
            for item in row:
                label, action, bg, fg, cs = item
                w = btn_w * cs + gap * (cs - 1)

                btn = tk.Canvas(
                    parent, width=w, height=btn_h,
                    bg=BG_MAIN, highlightthickness=0, cursor="hand2"
                )
                btn.grid(row=r, column=col_offset, columnspan=cs,
                         padx=(0, gap), pady=(0, gap), sticky="nw")

                # 圆角矩形
                radius = 14
                self._draw_round_rect(btn, 0, 0, w, btn_h, radius, bg)
                f = self.font_btn_large if cs > 1 or len(label) <= 2 else self.font_btn_small
                btn.create_text(w // 2, btn_h // 2, text=label, fill=fg, font=f)

                # 悬停颜色
                hover_bg = (HOVER_OP if bg == BG_BTN_OP
                            else HOVER_CLEAR if bg == BG_BTN_CLEAR
                            else HOVER_FUNC if bg == BG_BTN_FUNC
                            else HOVER_NUM)

                btn.bind("<Enter>",  lambda e, b=btn, hbg=hover_bg, lbl=label, fg=fg, f=f, w=w, h=btn_h, r=radius:
                         self._on_hover(b, hbg, lbl, fg, f, w, h, r))
                btn.bind("<Leave>",  lambda e, b=btn, obg=bg, lbl=label, fg=fg, f=f, w=w, h=btn_h, r=radius:
                         self._on_hover(b, obg, lbl, fg, f, w, h, r))
                btn.bind("<Button-1>", lambda e, a=action: self._on_click(a))

                col_offset += cs

    def _draw_round_rect(self, canvas, x1, y1, x2, y2, r, color):
        canvas.delete("all")
        canvas.create_arc(x1, y1, x1+2*r, y1+2*r, start=90,  extent=90,  fill=color, outline=color)
        canvas.create_arc(x2-2*r, y1, x2, y1+2*r, start=0,   extent=90,  fill=color, outline=color)
        canvas.create_arc(x1, y2-2*r, x1+2*r, y2, start=180, extent=90,  fill=color, outline=color)
        canvas.create_arc(x2-2*r, y2-2*r, x2, y2, start=270, extent=90,  fill=color, outline=color)
        canvas.create_rectangle(x1+r, y1, x2-r, y2, fill=color, outline=color)
        canvas.create_rectangle(x1, y1+r, x2, y2-r, fill=color, outline=color)

    def _on_hover(self, canvas, bg, label, fg, f, w, h, r):
        self._draw_round_rect(canvas, 0, 0, w, h, r, bg)
        canvas.create_text(w // 2, h // 2, text=label, fill=fg, font=f)

    # ── 键盘绑定 ────────────────────────────────────────────
    def _bind_keyboard(self):
        # 用具名 keysym 绑定特殊键，避免 <1><2> 被解析为鼠标按键
        self.bind("<Return>",    lambda e: self._on_click("="))
        self.bind("<KP_Enter>",  lambda e: self._on_click("="))
        self.bind("<BackSpace>", lambda e: self._on_click("DEL"))
        self.bind("<Escape>",    lambda e: self._on_click("AC"))
        # 所有可打印字符走 KeyPress → event.char
        self.bind("<KeyPress>",  self._on_key_press)

    def _on_key_press(self, event):
        CHAR_MAP = {
            '0':'0', '1':'1', '2':'2', '3':'3', '4':'4',
            '5':'5', '6':'6', '7':'7', '8':'8', '9':'9',
            '.':'.', '+':'+', '-':'-', '*':'*', '/':'/',
            '(':'(', ')':')', '%':'%',
        }
        if event.char in CHAR_MAP:
            self._on_click(CHAR_MAP[event.char])

    def _on_click(self, action):
        if action == "AC":
            self.expression = ""
            self.display_expr = ""
            self.result = "0"
            self.just_calculated = False
            self._update_display()
            return

        if action == "DEL":
            if self.just_calculated:
                # 刚算完就删除 → 清空
                self.expression = ""
                self.display_expr = ""
                self.result = "0"
                self.just_calculated = False
            elif self.tokens:
                # 按 token 删除（而非按字符）
                self.tokens.pop()
                self.expression = "".join(t[0] for t in self.tokens)
                self.display_expr = "".join(t[1] for t in self.tokens)
            self._update_display()
            return

        if action == "=":
            self._calculate()
            return

        # ── 计算完后继续输入 ──────────────────────────────────
        if self.just_calculated:
            if action in ("+", "-", "*", "/", "**", "pow"):
                # 继续用结果做左操作数
                self.tokens = [("(" + self.expression + ")", self.result)]
                self.expression = "(" + self.expression + ")"
                self.display_expr = self.result
            else:
                # 新表达式
                self.tokens = []
                self.expression = ""
                self.display_expr = ""
            self.just_calculated = False

        # ── 追加 token ────────────────────────────────────────
        expr_part, disp_part = self._resolve_token(action)
        self.tokens.append((expr_part, disp_part))
        self.expression += expr_part
        self.display_expr += disp_part
        self._update_display()

    def _resolve_token(self, action):
        """返回 (eval用字符串, 显示字符串)"""
        MAP = {
            "pi":    (str(math.pi),  "π"),
            "e":     (str(math.e),   "e"),
            "sin":   ("sin(",         "sin("),
            "cos":   ("cos(",         "cos("),
            "tan":   ("tan(",         "tan("),
            "sqrt":  ("sqrt(",        "√("),
            "log10": ("log10(",       "log("),
            "log":   ("log(",         "ln("),
            "pow":   ("**",           "^"),
            "sq":    ("**2",          "²"),
            "fact":  ("factorial(",   "("),   # 后面追 ) 靠用户
            "10pow": ("10**",         "10^"),
            "%":     ("/100",         "%"),
            "*":     ("*",            "×"),
            "/":     ("/",            "÷"),
            "-":     ("-",            "−"),
            "+":     ("+",            "+"),
            "(":     ("(",            "("),
            ")":     (")",            ")"),
            ".":     (".",            "."),
        }
        if action in MAP:
            return MAP[action]
        # 数字 0-9
        return (action, action)

    def _calculate(self):
        if not self.expression:
            return
        try:
            result = self._eval_with_mode(self.expression)
            fmt = self._format_num(result)
            self.lbl_history.config(text=self.display_expr + " =")
            self.lbl_result.config(text=fmt)
            # 保存结果供后续运算
            self.result = fmt
            self.expression = fmt          # 下次继续运算用数值字符串
            self.display_expr = fmt
            self.tokens = [(fmt, fmt)]
            self.just_calculated = True
        except ZeroDivisionError:
            self.lbl_history.config(text=self.display_expr + " =")
            self.lbl_result.config(text="除数不能为零")
            self._full_reset()
        except Exception as err:
            self.lbl_history.config(text=self.display_expr + " =")
            self.lbl_result.config(text="错误")
            self._full_reset()

    def _full_reset(self):
        self.expression = ""
        self.display_expr = ""
        self.tokens = []
        self.just_calculated = False

    def _eval_with_mode(self, expr):
        if self.angle_mode == "DEG":
            sin_f = lambda x: math.sin(math.radians(x))
            cos_f = lambda x: math.cos(math.radians(x))
            tan_f = lambda x: math.tan(math.radians(x))
        else:
            sin_f = math.sin
            cos_f = math.cos
            tan_f = math.tan

        return eval(expr, {"__builtins__": {}}, {
            "sin": sin_f, "cos": cos_f, "tan": tan_f,
            "sqrt": math.sqrt, "log": math.log, "log10": math.log10,
            "factorial": math.factorial, "pi": math.pi, "e": math.e,
            "abs": abs, "pow": pow,
        })

    def _format_num(self, n) -> str:
        if isinstance(n, complex):
            return str(n)
        if math.isinf(n):
            return "∞" if n > 0 else "-∞"
        if math.isnan(n):
            return "NaN"
        # 整数展示不带小数点
        if n == int(n) and abs(n) < 1e15:
            return str(int(n))
        # 最多10位有效数字，去尾零
        return f"{n:.10g}"

    def _update_display(self):
        if self.expression:
            self.lbl_history.config(text=self.display_expr)
            self.lbl_result.config(text=self.display_expr)
        else:
            self.lbl_history.config(text="")
            self.lbl_result.config(text=self.result)

    def _toggle_mode(self):
        self.angle_mode = "RAD" if self.angle_mode == "DEG" else "DEG"
        self.mode_btn.config(text=self.angle_mode)


if __name__ == "__main__":
    app = ScientificCalculator()
    # 居中显示
    app.update_idletasks()
    w, h = app.winfo_width(), app.winfo_height()
    sw, sh = app.winfo_screenwidth(), app.winfo_screenheight()
    app.geometry(f"+{(sw-w)//2}+{(sh-h)//2}")
    app.mainloop()
