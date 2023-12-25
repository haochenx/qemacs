/*
 * Regular Expression Engine
 *
 * Copyright (c) 2017-2018 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#if REBKT_ORIGINAL
#define GET_CHAR(c, cptr, cbuf_end)                                     \
    do {                                                                \
        if (cbuf_type == 0) {                                           \
            c = *cptr++;                                                \
        } else {                                                        \
            uint32_t __c1;                                              \
            c = *(const uint16_t *)(const void *)cptr;                  \
            cptr += 2;                                                  \
            if (c >= 0xd800 && c < 0xdc00 &&                            \
                cbuf_type == 2 && cptr < cbuf_end) {                    \
                __c1 = *(const uint16_t *)(const void *)cptr;           \
                if (__c1 >= 0xdc00 && __c1 < 0xe000) {                  \
                    c = (((c & 0x3ff) << 10) | (__c1 & 0x3ff)) + 0x10000; \
                    cptr += 2;                                          \
                }                                                       \
            }                                                           \
        }                                                               \
    } while (0)

#define PEEK_CHAR(c, cptr, cbuf_end)             \
    do {                                         \
        if (cbuf_type == 0) {                    \
            c = cptr[0];                         \
        } else {                                 \
            uint32_t __c1;                                              \
            c = ((const uint16_t *)(const void *)cptr)[0];              \
            if (c >= 0xd800 && c < 0xdc00 &&                            \
                cbuf_type == 2 && (cptr + 2) < cbuf_end) {              \
                __c1 = ((const uint16_t *)(const void *)cptr)[1];       \
                if (__c1 >= 0xdc00 && __c1 < 0xe000) {                  \
                    c = (((c & 0x3ff) << 10) | (__c1 & 0x3ff)) + 0x10000; \
                }                                                       \
            }                                                           \
        }                                                               \
    } while (0)

#define PEEK_PREV_CHAR(c, cptr, cbuf_start)      \
    do {                                         \
        if (cbuf_type == 0) {                    \
            c = cptr[-1];                        \
        } else {                                 \
            uint32_t __c1;                                              \
            c = ((const uint16_t *)(const void *)cptr)[-1];             \
            if (c >= 0xdc00 && c < 0xe000 &&                            \
                cbuf_type == 2 && (cptr - 4) >= cbuf_start) {           \
                __c1 = ((const uint16_t *)(const void *)cptr)[-2];      \
                if (__c1 >= 0xd800 && __c1 < 0xdc00 ) {                 \
                    c = (((__c1 & 0x3ff) << 10) | (c & 0x3ff)) + 0x10000; \
                }                                                       \
            }                                                           \
        }                                                               \
    } while (0)

#define GET_PREV_CHAR(c, cptr, cbuf_start)       \
    do {                                         \
        if (cbuf_type == 0) {                    \
            cptr--;                              \
            c = cptr[0];                         \
        } else {                                 \
            uint32_t __c1;                                              \
            cptr -= 2;                                                  \
            c = ((const uint16_t *)(const void *)cptr)[0];              \
            if (c >= 0xdc00 && c < 0xe000 &&                            \
                cbuf_type == 2 && cptr > cbuf_start) {                  \
                __c1 = ((const uint16_t *)(const void *)cptr)[-1];      \
                if (__c1 >= 0xd800 && __c1 < 0xdc00 ) {                 \
                    cptr -= 2;                                          \
                    c = (((__c1 & 0x3ff) << 10) | (c & 0x3ff)) + 0x10000; \
                }                                                       \
            }                                                           \
        }                                                               \
    } while (0)

#define PREV_CHAR(cptr, cbuf_start)       \
    do {                                  \
        if (cbuf_type == 0) {             \
            cptr--;                       \
        } else {                          \
            cptr -= 2;                    \
            if (cbuf_type == 2) {                                       \
                c = ((const uint16_t *)(const void *)cptr)[0];          \
                if (c >= 0xdc00 && c < 0xe000 && cptr > cbuf_start) {   \
                    c = ((const uint16_t *)(const void *)cptr)[-1];     \
                    if (c >= 0xd800 && c < 0xdc00)                      \
                        cptr -= 2;                                      \
                }                                                       \
            }                                                           \
        }                                                               \
    } while (0)
#define IF_USE_CBUF_TYPE(e) e
#else
struct EditBuffer;
unsigned int eb_nextc(struct EditBuffer *b, int offset, int *next_ptr);
unsigned int eb_prevc(struct EditBuffer *b, int offset, int *next_ptr);
#define GET_CHAR(c, cptr, cbuf_end)                     \
    do {                                                \
        int offset = cptr - s->cbuf;                    \
        struct EditBuffer *b = unconst(void *)s->cbuf;  \
        c = eb_nextc(b, offset, &offset);               \
        cptr = s->cbuf + offset;                        \
    } while (0)

#define PEEK_CHAR(c, cptr, cbuf_end)                    \
    do {                                                \
        int offset = cptr - s->cbuf;                    \
        struct EditBuffer *b = unconst(void *)s->cbuf;  \
        c = eb_nextc(b, offset, &offset);               \
    } while (0)

#define PEEK_PREV_CHAR(c, cptr, cbuf_start)             \
    do {                                                \
        int offset = cptr - s->cbuf;                    \
        struct EditBuffer *b = unconst(void *)s->cbuf;  \
        c = eb_prevc(b, offset, &offset);               \
    } while (0)

#define GET_PREV_CHAR(c, cptr, cbuf_start)              \
    do {                                                \
        int offset = cptr - s->cbuf;                    \
        struct EditBuffer *b = unconst(void *)s->cbuf;  \
        c = eb_prevc(b, offset, &offset);               \
        cptr = s->cbuf + offset;                        \
    } while (0)

#define PREV_CHAR(cptr, cbuf_start)                     \
    do {                                                \
        int offset = cptr - s->cbuf;                    \
        struct EditBuffer *b = unconst(void *)s->cbuf;  \
        eb_prevc(b, offset, &offset);                   \
        cptr = s->cbuf + offset;                        \
    } while (0)
#define IF_USE_CBUF_TYPE(e)
#endif

#ifdef REBKT_SUFFIX
#define lre_exec_backtrack0(suffix, ...) lre_exec_backtrack ## suffix (__VA_ARGS__)
#define lre_exec_backtrack(...) lre_exec_backtrack0(__VA_ARGS__)
#endif

/* return 1 if match, 0 if not match or -1 if error. */
static intptr_t lre_exec_backtrack(REBKT_SUFFIX,
                                   REExecContext *s, const uint8_t **capture,
                                   StackInt *stack, int stack_len,
                                   const uint8_t *pc, const uint8_t *cptr,
                                   BOOL no_recurse)
{
    int opcode, ret;
    IF_USE_CBUF_TYPE(int cbuf_type;)
    uint32_t val, c;
    const uint8_t *cbuf_end;

    IF_USE_CBUF_TYPE(cbuf_type = s->cbuf_type;)
    cbuf_end = s->cbuf_end;

    for(;;) {
        //        printf("top=%p: pc=%d\n", th_list.top, (int)(pc - (bc_buf + RE_HEADER_LEN)));
        opcode = *pc++;
        switch(opcode) {
        case REOP_match:
            {
                REExecState *rs;
                if (no_recurse)
                    return (intptr_t)cptr;
                ret = 1;
                goto recurse;
            no_match:
                if (no_recurse)
                    return 0;
                ret = 0;
            recurse:
                for(;;) {
                    if (s->state_stack_len == 0)
                        return ret;
                    rs = (REExecState *)(void *)(s->state_stack +
                                         (s->state_stack_len - 1) * s->state_size);
                    if (rs->type == RE_EXEC_STATE_SPLIT) {
                        if (!ret) {
                        pop_state:
                            memcpy(capture, rs->buf,
                                   sizeof(capture[0]) * 2 * s->capture_count);
                        pop_state1:
                            pc = rs->pc;
                            cptr = rs->cptr;
                            stack_len = rs->stack_len;
                            memcpy(stack, rs->buf + 2 * s->capture_count,
                                   stack_len * sizeof(stack[0]));
                            s->state_stack_len--;
                            break;
                        }
                    } else if (rs->type == RE_EXEC_STATE_GREEDY_QUANT) {
                        if (!ret) {
                            uint32_t char_count, i;
                            memcpy(capture, rs->buf,
                                   sizeof(capture[0]) * 2 * s->capture_count);
                            stack_len = rs->stack_len;
                            memcpy(stack, rs->buf + 2 * s->capture_count,
                                   stack_len * sizeof(stack[0]));
                            pc = rs->pc;
                            cptr = rs->cptr;
                            /* go backward */
                            char_count = get_u32(pc + 12);
                            for(i = 0; i < char_count; i++) {
                                PREV_CHAR(cptr, s->cbuf);
                            }
                            pc = (pc + 16) + (int)get_u32(pc);
                            rs->cptr = cptr;
                            rs->count--;
                            if (rs->count == 0) {
                                s->state_stack_len--;
                            }
                            break;
                        }
                    } else {
                        ret = ((rs->type == RE_EXEC_STATE_LOOKAHEAD && ret) ||
                               (rs->type == RE_EXEC_STATE_NEGATIVE_LOOKAHEAD && !ret));
                        if (ret) {
                            /* keep the capture in case of positive lookahead */
                            if (rs->type == RE_EXEC_STATE_LOOKAHEAD)
                                goto pop_state1;
                            else
                                goto pop_state;
                        }
                    }
                    s->state_stack_len--;
                }
            }
            break;
        case REOP_char32:
            val = get_u32(pc);
            pc += 4;
            goto test_char;
        case REOP_char:
            val = get_u16(pc);
            pc += 2;
        test_char:
            if (cptr >= cbuf_end)
                goto no_match;
            GET_CHAR(c, cptr, cbuf_end);
            if (s->ignore_case) {
                c = lre_canonicalize(c, s->is_utf16);
            }
            if (val != c)
                goto no_match;
            break;
        case REOP_split_goto_first:
        case REOP_split_next_first:
            {
                const uint8_t *pc1;

                val = get_u32(pc);
                pc += 4;
                if (opcode == REOP_split_next_first) {
                    pc1 = pc + (int)val;
                } else {
                    pc1 = pc;
                    pc = pc + (int)val;
                }
                ret = push_state(s, capture, stack, stack_len,
                                 pc1, cptr, RE_EXEC_STATE_SPLIT, 0);
                if (ret < 0)
                    return -1;
                break;
            }
        case REOP_lookahead:
        case REOP_negative_lookahead:
            val = get_u32(pc);
            pc += 4;
            ret = push_state(s, capture, stack, stack_len,
                             pc + (int)val, cptr,
                             RE_EXEC_STATE_LOOKAHEAD + opcode - REOP_lookahead,
                             0);
            if (ret < 0)
                return -1;
            break;

        case REOP_goto:
            val = get_u32(pc);
            pc += 4 + (int)val;
            break;
        case REOP_line_start:
            if (cptr == s->cbuf) {
                if (s->is_not_bol)
                    goto no_match;
                break;
            }
            if (!s->multi_line)
                goto no_match;
            PEEK_PREV_CHAR(c, cptr, s->cbuf);
            if (!is_line_terminator(c))
                goto no_match;
            break;
        case REOP_line_end:
            if (cptr == cbuf_end) {
                if (s->is_not_eol)
                    goto no_match;
                break;
            }
            if (!s->multi_line)
                goto no_match;
            PEEK_CHAR(c, cptr, cbuf_end);
            if (!is_line_terminator(c))
                goto no_match;
            break;
        case REOP_dot:
            if (cptr == cbuf_end)
                goto no_match;
            GET_CHAR(c, cptr, cbuf_end);
            if (is_line_terminator(c))
                goto no_match;
            break;
        case REOP_any:
            if (cptr == cbuf_end)
                goto no_match;
            GET_CHAR(c, cptr, cbuf_end);
            break;
        case REOP_save_start:
        case REOP_save_end:
            val = *pc++;
            assert(val < (uint32_t)s->capture_count);
            capture[2 * val + opcode - REOP_save_start] = (const uint8_t *)cptr;
            break;
        case REOP_save_reset:
            {
                uint32_t val2;
                val = pc[0];
                val2 = pc[1];
                pc += 2;
                assert(val2 < (uint32_t)s->capture_count);
                while (val <= val2) {
                    capture[2 * val] = NULL;
                    capture[2 * val + 1] = NULL;
                    val++;
                }
            }
            break;
        case REOP_push_i32:
            val = get_u32(pc);
            pc += 4;
            stack[stack_len++] = val;
            break;
        case REOP_drop:
            stack_len--;
            break;
        case REOP_loop:
            val = get_u32(pc);
            pc += 4;
            if (--stack[stack_len - 1] != 0) {
                pc += (int)val;
            }
            break;
        case REOP_push_char_pos:
            stack[stack_len++] = (uintptr_t)cptr;
            break;
        case REOP_bne_char_pos:
            val = get_u32(pc);
            pc += 4;
            if (stack[--stack_len] != (uintptr_t)cptr)
                pc += (int)val;
            break;
        case REOP_word_boundary:
        case REOP_not_word_boundary:
            {
                BOOL v1, v2;
                /* char before */
                if (cptr == s->cbuf) {
                    v1 = s->is_not_bow;
                } else {
                    PEEK_PREV_CHAR(c, cptr, s->cbuf);
                    v1 = is_word_char(c);
                }
                /* current char */
                if (cptr >= cbuf_end) {
                    v2 = s->is_not_eow;
                } else {
                    PEEK_CHAR(c, cptr, cbuf_end);
                    v2 = is_word_char(c);
                }
                if (v1 ^ v2 ^ (REOP_not_word_boundary - opcode))
                    goto no_match;
            }
            break;
        case REOP_back_reference:
        case REOP_backward_back_reference:
            {
                const uint8_t *cptr1, *cptr1_end, *cptr1_start;
                uint32_t c1, c2;

                val = *pc++;
                if (val >= (uint32_t)s->capture_count)
                    goto no_match;
                cptr1_start = capture[2 * val];
                cptr1_end = capture[2 * val + 1];
                if (!cptr1_start || !cptr1_end)
                    break;
                if (opcode == REOP_back_reference) {
                    cptr1 = cptr1_start;
                    while (cptr1 < cptr1_end) {
                        if (cptr >= cbuf_end)
                            goto no_match;
                        GET_CHAR(c1, cptr1, cptr1_end);
                        GET_CHAR(c2, cptr, cbuf_end);
                        if (s->ignore_case) {
                            c1 = lre_canonicalize(c1, s->is_utf16);
                            c2 = lre_canonicalize(c2, s->is_utf16);
                        }
                        if (c1 != c2)
                            goto no_match;
                    }
                } else {
                    cptr1 = cptr1_end;
                    while (cptr1 > cptr1_start) {
                        if (cptr == s->cbuf)
                            goto no_match;
                        GET_PREV_CHAR(c1, cptr1, cptr1_start);
                        GET_PREV_CHAR(c2, cptr, s->cbuf);
                        if (s->ignore_case) {
                            c1 = lre_canonicalize(c1, s->is_utf16);
                            c2 = lre_canonicalize(c2, s->is_utf16);
                        }
                        if (c1 != c2)
                            goto no_match;
                    }
                }
            }
            break;
        case REOP_range:
            {
                int n;
                uint32_t low, high, idx_min, idx_max, idx;

                n = get_u16(pc); /* n must be >= 1 */
                pc += 2;
                if (cptr >= cbuf_end)
                    goto no_match;
                GET_CHAR(c, cptr, cbuf_end);
                if (s->ignore_case) {
                    c = lre_canonicalize(c, s->is_utf16);
                }
                idx_min = 0;
                low = get_u16(pc + 0 * 4);
                if (c < low)
                    goto no_match;
                idx_max = n - 1;
                high = get_u16(pc + idx_max * 4 + 2);
                /* 0xffff in for last value means +infinity */
                if (unlikely(c >= 0xffff) && high == 0xffff)
                    goto range_match;
                if (c > high)
                    goto no_match;
                while (idx_min <= idx_max) {
                    idx = (idx_min + idx_max) / 2;
                    low = get_u16(pc + idx * 4);
                    high = get_u16(pc + idx * 4 + 2);
                    if (c < low)
                        idx_max = idx - 1;
                    else if (c > high)
                        idx_min = idx + 1;
                    else
                        goto range_match;
                }
                goto no_match;
            range_match:
                pc += 4 * n;
            }
            break;
        case REOP_range32:
            {
                int n;
                uint32_t low, high, idx_min, idx_max, idx;

                n = get_u16(pc); /* n must be >= 1 */
                pc += 2;
                if (cptr >= cbuf_end)
                    goto no_match;
                GET_CHAR(c, cptr, cbuf_end);
                if (s->ignore_case) {
                    c = lre_canonicalize(c, s->is_utf16);
                }
                idx_min = 0;
                low = get_u32(pc + 0 * 8);
                if (c < low)
                    goto no_match;
                idx_max = n - 1;
                high = get_u32(pc + idx_max * 8 + 4);
                if (c > high)
                    goto no_match;
                while (idx_min <= idx_max) {
                    idx = (idx_min + idx_max) / 2;
                    low = get_u32(pc + idx * 8);
                    high = get_u32(pc + idx * 8 + 4);
                    if (c < low)
                        idx_max = idx - 1;
                    else if (c > high)
                        idx_min = idx + 1;
                    else
                        goto range32_match;
                }
                goto no_match;
            range32_match:
                pc += 8 * n;
            }
            break;
        case REOP_prev:
            /* go to the previous char */
            if (cptr == s->cbuf)
                goto no_match;
            PREV_CHAR(cptr, s->cbuf);
            break;
        case REOP_simple_greedy_quant:
            {
                uint32_t next_pos, quant_min, quant_max;
                size_t q;
                intptr_t res;
                const uint8_t *pc1;

                next_pos = get_u32(pc);
                quant_min = get_u32(pc + 4);
                quant_max = get_u32(pc + 8);
                pc += 16;
                pc1 = pc;
                pc += (int)next_pos;

                q = 0;
                for(;;) {
                  res = lre_exec_backtrack(REBKT_SUFFIX, s, capture, stack, stack_len,
                                             pc1, cptr, TRUE);
                    if (res == -1)
                        return res;
                    if (!res)
                        break;
                    cptr = (uint8_t *)res;
                    q++;
                    if (q >= quant_max && quant_max != INT32_MAX)
                        break;
                }
                if (q < quant_min)
                    goto no_match;
                if (q > quant_min) {
                    /* will examine all matches down to quant_min */
                    ret = push_state(s, capture, stack, stack_len,
                                     pc1 - 16, cptr,
                                     RE_EXEC_STATE_GREEDY_QUANT,
                                     q - quant_min);
                    if (ret < 0)
                        return -1;
                }
            }
            break;
        default:
            abort();
        }
    }
}

#undef GET_CHAR
#undef PEEK_CHAR
#undef PEEK_PREV_CHAR
#undef GET_PREV_CHAR
#undef PREV_CHAR
#undef IF_USE_CBUF_TYPE

#ifdef lre_exec_backtrack
#undef lre_exec_backtrack
#endif
