#include "GUI.h"

#pragma region Codes Page
static const BPP1_DAT acFont8_1_160[8] { /* code 160 */
   ________________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________};
static const BPP1_DAT acFont8_1_161[8] { /* code 161 */
   ________________,
   __XX____________,
   ________________,
   __XX____________,
   __XX____________,
   __XX____________,
   __XX____________,
   __XX____________};
static const BPP1_DAT acFont8_1_162[8] { /* code 162 */
   ________________,
   ____XX__________,
   __XXXXXX________,
   XX__XX__________,
   XX__XX__________,
   XX__XX__XX______,
   __XXXXXX________,
   ____XX__________};
static const BPP1_DAT acFont8_1_163[8] { /* code 163 */
   ____XXXX________,
   __XX____________,
   __XX____________,
   XXXXXX__________,
   __XX____________,
   __XX____XX______,
   XX__XXXX________,
   ________________};
static const BPP1_DAT acFont8_1_164[8] { /* code 164 */
   ________________,
   ________________,
   XX______XX______,
   __XXXXXX________,
   __XX__XX________,
   __XXXXXX________,
   XX______XX______,
   ________________};
static const BPP1_DAT acFont8_1_165[8] { /* code 165 */
   XX______XX______,
   __XX__XX________,
   XXXXXXXXXX______,
   ____XX__________,
   XXXXXXXXXX______,
   ____XX__________,
   ____XX__________,
   ________________};
static const BPP1_DAT acFont8_1_166[8] { /* code 166 */
   ____XX__________,
   ____XX__________,
   ____XX__________,
   ________________,
   ____XX__________,
   ____XX__________,
   ____XX__________,
   ________________};
static const BPP1_DAT acFont8_1_167[8] { /* code 167 */
   ____XXXX________,
   __XX____XX______,
   ____XX__________,
   __XX__XX________,
   ____XX__________,
   XX____XX________,
   __XXXX__________,
   ________________};
static const BPP1_DAT acFont8_1_168[8] { /* code 168 */
   XX______XX______,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________};
static const BPP1_DAT acFont8_1_169[8] { /* code 169 */
   XXXXXXXXXX______,
   XX______XX______,
   XX__XX__XX______,
   XX__XXXXXX______,
   XX__XX__XX______,
   XX______XX______,
   XXXXXXXXXX______,
   ________________};
static const BPP1_DAT acFont8_1_170[8] { /* code 170 */
   __XXXXXX________,
   ________XX______,
   __XXXXXXXX______,
   XX______XX______,
   __XXXXXX________,
   ________________,
   ________________,
   ________________};
static const BPP1_DAT acFont8_1_171[8] { /* code 171 */
   ________________,
   ____XX__XX______,
   __XX__XX________,
   XX__XX__________,
   __XX__XX________,
   ____XX__XX______,
   ________________,
   ________________};
static const BPP1_DAT acFont8_1_172[8] { /* code 172 */
   ________________,
   ________________,
   ________________,
   XXXXXXXXXX______,
   ________XX______,
   ________XX______,
   ________________,
   ________________};
static const BPP1_DAT acFont8_1_173[8] { /* code 173 */
   ________________,
   ________________,
   ________________,
   __XXXXXX________,
   ________________,
   ________________,
   ________________,
   ________________};
static const BPP1_DAT acFont8_1_174[8] { /* code 174 */
   XXXXXXXXXX______,
   XX______XX______,
   XX__XX__XX______,
   XX______XX______,
   XX____XXXX______,
   XX__XX__XX______,
   XXXXXXXXXX______,
   ________________};
static const BPP1_DAT acFont8_1_175[8] { /* code 175 */
   XXXXXXXXXX______,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________};
static const BPP1_DAT acFont8_1_176[8] { /* code 176 */
   ____XX__________,
   __XX__XX________,
   ____XX__________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________};
static const BPP1_DAT acFont8_1_177[8] { /* code 177 */
   ________________,
   ____XX__________,
   __XXXXXX________,
   ____XX__________,
   ________________,
   __XXXXXX________,
   ________________,
   ________________};
static const BPP1_DAT acFont8_1_178[8] { /* code 178 */
   __XXXX__________,
   XX____XX________,
   ____XX__________,
   __XX____________,
   XXXXXXXX________,
   ________________,
   ________________,
   ________________};
static const BPP1_DAT acFont8_1_179[8] { /* code 179 */
   XXXXXX__________,
   ______XX________,
   __XXXX__________,
   ______XX________,
   XXXXXX__________,
   ________________,
   ________________,
   ________________};
static const BPP1_DAT acFont8_1_180[8] { /* code 180 */
   ______XXXX______,
   ____XX__________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________,
   ________________};
static const BPP1_DAT acFont8_1_181[8] { /* code 181 */
   ________________,
   ________________,
   XX______XX______,
   XX______XX______,
   XX______XX______,
   XXXX____XX______,
   XX__XXXX________,
   XX______________};
static const BPP1_DAT acFont8_1_182[8] { /* code 182 */
   __XXXXXXXX______,
   XX____XXXX______,
   XX____XXXX______,
   __XXXXXXXX______,
   ______XXXX______,
   ______XXXX______,
   ______XXXX______,
   ________________};
static const BPP1_DAT acFont8_1_183[8] { /* code 183 */
   ________________,
   ________________,
   ________________,
   ____XXXX________,
   ____XXXX________,
   ________________,
   ________________,
   ________________};
static const BPP1_DAT acFont8_1_184[8] { /* code 184 */
  ________________,
  ________________,
  ________________,
  ________________,
  ________________,
  ________________,
  ____XX__________,
  __XXXX__________};
static const BPP1_DAT acFont8_1_185[8] { /* code 185 */
   __XX____________,
   XXXX____________,
   __XX____________,
   __XX____________,
   XXXXXX__________,
   ________________,
   ________________,
   ________________};
static const BPP1_DAT acFont8_1_186[8] { /* code 186 */
   __XXXXXX________,
   XX______XX______,
   XX______XX______,
   XX______XX______,
   __XXXXXX________,
   ________________,
   ________________,
   ________________};
static const BPP1_DAT acFont8_1_187[8] { /* code 187 */
   ________________,
   XX__XX__________,
   __XX__XX________,
   ____XX__XX______,
   __XX__XX________,
   XX__XX__________,
   ________________,
   ________________};
static const BPP1_DAT acFont8_1_188[8] { /* code 188 */
   XX______XX______,
   XX____XX________,
   XX__XX__________,
   __XX__XX________,
   XX__XXXX________,
   __XX__XX________,
   __XXXXXXXX______,
   ______XX________};
static const BPP1_DAT acFont8_1_189[8] { /* code 189 */
   XX______XX______,
   XX____XX________,
   XX__XX__________,
   __XX____________,
   XX__XXXXXX______,
   ________XX______,
   ______XX________,
   ____XXXXXX______};
static const BPP1_DAT acFont8_1_190[8] { /* code 190 */
   XXXX____________,
   __XX____________,
   XXXX____________,
   __XX____XX______,
   XXXX__XXXX______,
   ____XX__XX______,
   ____XXXXXX______,
   ________XX______};
static const BPP1_DAT acFont8_1_191[8] { /* code 191 */
  ________________,
  ____XX__________,
  ________________,
  ____XX__________,
  ____XX__________,
  __XX____________,
  XX______XX______,
  __XXXXXX________};
static const BPP1_DAT acFont8_1_192[8] { /* code 192 */
  ________________,
  ____XX__________,
  ______XX________,
  ____XX__XX______,
  __XX______XX____,
  __XXXXXXXXXX____,
  XX__________XX__,
  XX__________XX__};
static const BPP1_DAT acFont8_1_193[8] { /* code 193 */
  ________________,
  ________XX______,
  ______XX________,
  ____XX__XX______,
  __XX______XX____,
  __XXXXXXXXXX____,
  XX__________XX__,
  XX__________XX__};
static const BPP1_DAT acFont8_1_194[8] { /* code 194 */
  ________________,
  ______XX________,
  ____XX__XX______,
  ____XXXXXX______,
  __XX______XX____,
  __XXXXXXXXXX____,
  XX__________XX__,
  XX__________XX__};
static const BPP1_DAT acFont8_1_195[8] { /* code 195 */
  ________________,
  ____XXXX____XX__,
  __XX____XXXX____,
  ____XXXXXX______,
  __XX______XX____,
  __XXXXXXXXXX____,
  XX__________XX__,
  XX__________XX__};
static const BPP1_DAT acFont8_1_196[8] { /* code 196 */
  ________________,
  __XX______XX____,
  ______XX________,
  ____XX__XX______,
  __XX______XX____,
  __XXXXXXXXXX____,
  XX__________XX__,
  XX__________XX__};
static const BPP1_DAT acFont8_1_197[8] { /* code 197 */
  ________________,
  ____XXXXXX______,
  ____XX__XX______,
  ____XXXXXX______,
  __XX______XX____,
  __XXXXXXXXXX____,
  XX__________XX__,
  XX__________XX__};
static const BPP1_DAT acFont8_1_198[8] { /* code 198 */
  ________________,
  ______XXXXXXXX__,
  ____XX__XX______,
  __XX____XX______,
  __XX____XXXXXX__,
  __XXXXXXXX______,
  XX______XX______,
  XX______XXXXXX__};
static const BPP1_DAT acFont8_1_199[8] { /* code 199 */
  ________________,
  __XXXXXXXX______,
  XX________XX____,
  XX______________,
  XX______________,
  XX________XX____,
  __XXXXXXXXXX____,
  ________XX______};
static const BPP1_DAT acFont8_1_200[8] { /* code 200 */
  __XX____________,
  XXXXXXXXXX______,
  XX______________,
  XX______________,
  XXXXXXXX________,
  XX______________,
  XX______________,
  XXXXXXXXXX______};
static const BPP1_DAT acFont8_1_201[8] { /* code 201 */
  ______XX________,
  XXXXXXXXXX______,
  XX______________,
  XX______________,
  XXXXXXXX________,
  XX______________,
  XX______________,
  XXXXXXXXXX______};
static const BPP1_DAT acFont8_1_202[8] { /* code 202 */
  __XXXXXX________,
  XXXXXXXXXX______,
  XX______________,
  XX______________,
  XXXXXXXX________,
  XX______________,
  XX______________,
  XXXXXXXXXX______};
static const BPP1_DAT acFont8_1_203[8] { /* code 203 */
  __XX__XX________,
  XXXXXXXXXX______,
  XX______________,
  XX______________,
  XXXXXXXX________,
  XX______________,
  XX______________,
  XXXXXXXXXX______};
static const BPP1_DAT acFont8_1_204[8] { /* code 204 */
  ________________,
  XX______________,
  __XX____________,
  ________________,
  XX______________,
  XX______________,
  XX______________,
  ________________};
static const BPP1_DAT acFont8_1_205[8] { /* code 205 */
  ________________,
  __XX____________,
  XX______________,
  ________________,
  XX______________,
  XX______________,
  XX______________,
  ________________};
static const BPP1_DAT acFont8_1_206[8] { /* code 206 */
  ________________,
  __XX____________,
  XX__XX__________,
  ________________,
  __XX____________,
  __XX____________,
  __XX____________,
  ________________};
static const BPP1_DAT acFont8_1_207[8] { /* code 207 */
  ________________,
  XX__XX__________,
  ________________,
  __XX____________,
  __XX____________,
  __XX____________,
  __XX____________,
  ________________};
static const BPP1_DAT acFont8_1_208[8] { /* code 208 */
  __XXXXXXXX______,
  __XX______XX____,
  __XX________XX__,
  XXXXXXXX____XX__,
  __XX________XX__,
  __XX______XX____,
  __XXXXXXXX______,
  ________________};
static const BPP1_DAT acFont8_1_209[8] { /* code 209 */
  __XX__XX________,
  XX________XX____,
  XXXX______XX____,
  XX__XX____XX____,
  XX____XX__XX____,
  XX______XXXX____,
  XX________XX____,
  ________________};
static const BPP1_DAT acFont8_1_210[8] { /* code 210 */
  __XX____________,
  ____XX__________,
  __XXXXXXXX______,
  XX________XX____,
  XX________XX____,
  XX________XX____,
  __XXXXXXXX______,
  ________________};
static const BPP1_DAT acFont8_1_211[8] { /* code 211 */
  ________XX______,
  ______XX________,
  __XXXXXXXX______,
  XX________XX____,
  XX________XX____,
  XX________XX____,
  __XXXXXXXX______,
  ________________};
static const BPP1_DAT acFont8_1_212[8] { /* code 212 */
  ____XX__________,
  __XX__XX________,
  __XXXXXXXX______,
  XX________XX____,
  XX________XX____,
  XX________XX____,
  __XXXXXXXX______,
  ________________};
static const BPP1_DAT acFont8_1_213[8] { /* code 213 */
  ____XX__XX______,
  __XX__XX________,
  __XXXXXXXX______,
  XX________XX____,
  XX________XX____,
  XX________XX____,
  __XXXXXXXX______,
  ________________};
static const BPP1_DAT acFont8_1_214[8] { /* code 214 */
  __XX____XX______,
  ________________,
  __XXXXXXXX______,
  XX________XX____,
  XX________XX____,
  XX________XX____,
  __XXXXXXXX______,
  ________________};
static const BPP1_DAT acFont8_1_215[8] { /* code 215 */
   ________________,
   XX______XX______,
   __XX__XX________,
   ____XX__________,
   __XX__XX________,
   XX______XX______,
   ________________,
   ________________};
static const BPP1_DAT acFont8_1_216[8] { /* code 216 */
  ____XXXXXX__XX__,
  __XX______XX____,
  XX______XX__XX__,
  XX____XX____XX__,
  XX__XX______XX__,
  __XX______XX____,
  XX__XXXXXX______,
  ________________};
static const BPP1_DAT acFont8_1_217[8] { /* code 217 */
  ____XX__________,
  ______XX________,
  XX________XX____,
  XX________XX____,
  XX________XX____,
  XX________XX____,
  __XXXXXXXX______,
  ________________};
static const BPP1_DAT acFont8_1_218[8] { /* code 218 */
  ______XX________,
  ____XX__________,
  XX________XX____,
  XX________XX____,
  XX________XX____,
  XX________XX____,
  __XXXXXXXX______,
  ________________};
static const BPP1_DAT acFont8_1_219[8] { /* code 219 */
  ____XX__________,
  __XX__XX________,
  XX________XX____,
  XX________XX____,
  XX________XX____,
  XX________XX____,
  __XXXXXXXX______,
  ________________};
static const BPP1_DAT acFont8_1_220[8] { /* code 220 */
  __XX____XX______,
  ________________,
  XX________XX____,
  XX________XX____,
  XX________XX____,
  XX________XX____,
  __XXXXXXXX______,
  ________________};
static const BPP1_DAT acFont8_1_221[8] { /* code 221 */
  ______XX________,
  XX__XX______XX__,
  __XX______XX____,
  ____XX__XX______,
  ______XX________,
  ______XX________,
  ______XX________,
  ________________};
static const BPP1_DAT acFont8_1_222[8] { /* code 222 */
  ________________,
  XX______________,
  XXXXXXXXXX______,
  XX________XX____,
  XX________XX____,
  XXXXXXXXXX______,
  XX______________,
  ________________};
static const BPP1_DAT acFont8_1_223[8] { /* code 223 */
  ________________,
  __XXXXXX________,
  XX______XX______,
  XX____XX________,
  XX____XXXX______,
  XX__XX____XX____,
  XX____XXXX______,
  ________________};
static const BPP1_DAT acFont8_1_224[8] { /* code 224 */
  ____XX__________,
  ______XX________,
  __XXXXXX________,
  ________XX______,
  __XXXXXXXX______,
  XX______XX______,
  __XXXXXXXX______,
  ________________};
static const BPP1_DAT acFont8_1_225[8] { /* code 225 */
  ______XX________,
  ____XX__________,
  __XXXXXX________,
  ________XX______,
  __XXXXXXXX______,
  XX______XX______,
  __XXXXXXXX______,
  ________________};
static const BPP1_DAT acFont8_1_226[8] { /* code 226 */
  ____XX__________,
  __XX__XX________,
  __XXXXXX________,
  ________XX______,
  __XXXXXXXX______,
  XX______XX______,
  __XXXXXXXX______,
  ________________};
static const BPP1_DAT acFont8_1_227[8] { /* code 227 */
  ____XX__XX______,
  __XX__XX________,
  __XXXXXX________,
  ________XX______,
  __XXXXXXXX______,
  XX______XX______,
  __XXXXXXXX______,
  ________________};
static const BPP1_DAT acFont8_1_228[8] { /* code 228 */
  __XX__XX________,
  ________________,
  __XXXXXX________,
  ________XX______,
  __XXXXXXXX______,
  XX______XX______,
  __XXXXXXXX______,
  ________________};
static const BPP1_DAT acFont8_1_229[8] { /* code 229 */
  __XXXXXX________,
  __XX__XX________,
  __XXXXXX________,
  ________XX______,
  __XXXXXXXX______,
  XX______XX______,
  __XXXXXXXX______,
  ________________};
static const BPP1_DAT acFont8_1_230[16] { /* code 230 */
  ________________,________________,
  __XXXXXX__XXXXXX,________________,
  ________XX______,XX______________,
  __XXXXXXXXXXXXXX,XX______________,
  XX______XX______,________________,
  XX______XX______,XX______________,
  __XXXXXX__XXXXXX,________________,
  ________________,________________};
static const BPP1_DAT acFont8_1_231[8] { /* code 231 */
  ________________,
  ________________,
  __XXXXXX________,
  XX______XX______,
  XX______________,
  XX______XX______,
  __XXXXXX________,
  ______XX________};
static const BPP1_DAT acFont8_1_232[8] { /* code 232 */
  __XX____________,
  ____XX__________,
  __XXXXXX________,
  XX______XX______,
  XXXXXXXXXX______,
  XX______________,
  __XXXXXX________,
  ________________};
static const BPP1_DAT acFont8_1_233[8] { /* code 233 */
  ______XX________,
  ____XX__________,
  __XXXXXX________,
  XX______XX______,
  XXXXXXXXXX______,
  XX______________,
  __XXXXXX________,
  ________________};
static const BPP1_DAT acFont8_1_234[8] { /* code 234 */
  ____XX__________,
  __XX__XX________,
  __XXXXXX________,
  XX______XX______,
  XXXXXXXXXX______,
  XX______________,
  __XXXXXX________,
  ________________};
static const BPP1_DAT acFont8_1_235[8] { /* code 235 */
  __XX__XX________,
  ________________,
  __XXXXXX________,
  XX______XX______,
  XXXXXXXXXX______,
  XX______________,
  __XXXXXX________,
  ________________};
static const BPP1_DAT acFont8_1_236[8] { /* code 236 */
  XX______________,
  __XX____________,
  ________________,
  XX______________,
  XX______________,
  XX______________,
  XX______________,
  ________________};
static const BPP1_DAT acFont8_1_237[8] { /* code 237 */
  __XX____________,
  XX______________,
  ________________,
  XX______________,
  XX______________,
  XX______________,
  XX______________,
  ________________};
static const BPP1_DAT acFont8_1_238[8] { /* code 238 */
  __XX____________,
  XX__XX__________,
  ________________,
  __XX____________,
  __XX____________,
  __XX____________,
  __XX____________,
  ________________};
static const BPP1_DAT acFont8_1_239[8] { /* code 239 */
  XX__XX__________,
  ________________,
  __XX____________,
  __XX____________,
  __XX____________,
  __XX____________,
  __XX____________,
  ________________};
static const BPP1_DAT acFont8_1_240[8] { /* code 240 */
  ________________,
  ____XXXXXX______,
  __XX__XX________,
  __XXXXXXXX______,
  XX______XX______,
  XX______XX______,
  __XXXXXX________,
  ________________};
static const BPP1_DAT acFont8_1_241[8] { /* code 241 */
  ____XX__XX______,
  __XX__XX________,
  XXXXXXXX________,
  XX______XX______,
  XX______XX______,
  XX______XX______,
  XX______XX______,
  ________________};
static const BPP1_DAT acFont8_1_242[8] { /* code 242 */
  __XX____________,
  ____XX__________,
  __XXXXXX________,
  XX______XX______,
  XX______XX______,
  XX______XX______,
  __XXXXXX________,
  ________________};
static const BPP1_DAT acFont8_1_243[8] { /* code 243 */
  ______XX________,
  ____XX__________,
  __XXXXXX________,
  XX______XX______,
  XX______XX______,
  XX______XX______,
  __XXXXXX________,
  ________________};
static const BPP1_DAT acFont8_1_244[8] { /* code 244 */
  ____XX__________,
  __XX__XX________,
  __XXXXXX________,
  XX______XX______,
  XX______XX______,
  XX______XX______,
  __XXXXXX________,
  ________________};
static const BPP1_DAT acFont8_1_245[8] { /* code 245 */
  ____XX__XX______,
  __XX__XX________,
  __XXXXXX________,
  XX______XX______,
  XX______XX______,
  XX______XX______,
  __XXXXXX________,
  ________________};
static const BPP1_DAT acFont8_1_246[8] { /* code 246 */
  __XX__XX________,
  ________________,
  __XXXXXX________,
  XX______XX______,
  XX______XX______,
  XX______XX______,
  __XXXXXX________,
  ________________};
static const BPP1_DAT acFont8_1_247[8] { /* code 247 */
   ________________,
   ____XX__________,
   ________________,
   XXXXXXXXXX______,
   ________________,
   ____XX__________,
   ________________,
   ________________};
static const BPP1_DAT acFont8_1_248[8] { /* code 248 */
  ________________,
  ________________,
  __XXXX__XX______,
  XX____XXXX______,
  XX__XX__XX______,
  XXXX____XX______,
  XX__XXXX________,
  ________________};
static const BPP1_DAT acFont8_1_249[8] { /* code 249 */
  __XX____________,
  ____XX__________,
  ________________,
  XX______XX______,
  XX______XX______,
  XX____XXXX______,
  __XXXX__XX______,
  ________________};
static const BPP1_DAT acFont8_1_250[8] { /* code 250 */
  ______XX________,
  ____XX__________,
  ________________,
  XX______XX______,
  XX______XX______,
  XX____XXXX______,
  __XXXX__XX______,
  ________________};
static const BPP1_DAT acFont8_1_251[8] { /* code 251 */
  ____XX__________,
  __XX__XX________,
  ________________,
  XX______XX______,
  XX______XX______,
  XX____XXXX______,
  __XXXX__XX______,
  ________________};
static const BPP1_DAT acFont8_1_252[8] { /* code 252 */
  ________________,
  __XX__XX________,
  ________________,
  XX______XX______,
  XX______XX______,
  XX____XXXX______,
  __XXXX__XX______,
  ________________};
static const BPP1_DAT acFont8_1_253[8] { /* code 253 */
  ______XX________,
  ____XX__________,
  XX______XX______,
  XX______XX______,
  __XX__XX________,
  ____XX__________,
  ____XX__________,
  __XX____________};
static const BPP1_DAT acFont8_1_254[8] { /* code 254 */
  ________________,
  XX______________,
  XXXXXXXX________,
  XX______XX______,
  XX______XX______,
  XXXXXXXX________,
  XX______________,
  ________________};
static const BPP1_DAT acFont8_1_255[8] { /* code 255 */
  ________________,
  ____XX__XX______,
  __XX__XX________,
  XX______XX______,
  __XX__XX________,
  ____XX__________,
  ____XX__________,
  __XX____________};
#pragma endregion

static const FontProp::CharInfo Font8_1_CharInfo[96] {
	 {   3,   3,  1, acFont8_1_160 } /* code 160 */
	,{   2,   2,  1, acFont8_1_161 } /* code 161 */
	,{   6,   6,  1, acFont8_1_162 } /* code 162 */
	,{   6,   6,  1, acFont8_1_163 } /* code 163 */
	,{   7,   7,  1, acFont8_1_164 } /* code 164 */
	,{   6,   6,  1, acFont8_1_165 } /* code 165 */
	,{   2,   2,  1, acFont8_1_166 } /* code 166 */
	,{   6,   6,  1, acFont8_1_167 } /* code 167 */
	,{   4,   4,  1, acFont8_1_168 } /* code 168 */
	,{   8,   8,  1, acFont8_1_169 } /* code 169 */
	,{   4,   4,  1, acFont8_1_170 } /* code 170 */
	,{   6,   6,  1, acFont8_1_171 } /* code 171 */
	,{   6,   6,  1, acFont8_1_172 } /* code 172 */
	,{   4,   4,  1, acFont8_1_173 } /* code 173 */
	,{   8,   8,  1, acFont8_1_174 } /* code 174 */
	,{   6,   6,  1, acFont8_1_175 } /* code 175 */
	,{   4,   4,  1, acFont8_1_176 } /* code 176 */
	,{   6,   6,  1, acFont8_1_177 } /* code 177 */
	,{   4,   4,  1, acFont8_1_178 } /* code 178 */
	,{   4,   4,  1, acFont8_1_179 } /* code 179 */
	,{   4,   4,  1, acFont8_1_180 } /* code 180 */
	,{   6,   6,  1, acFont8_1_181 } /* code 181 */
	,{   6,   6,  1, acFont8_1_182 } /* code 182 */
	,{   3,   3,  1, acFont8_1_183 } /* code 183 */
	,{   4,   4,  1, acFont8_1_184 } /* code 184 */
	,{   4,   4,  1, acFont8_1_185 } /* code 185 */
	,{   5,   5,  1, acFont8_1_186 } /* code 186 */
	,{   6,   6,  1, acFont8_1_187 } /* code 187 */
	,{   9,   9,  2, acFont8_1_188 } /* code 188 */
	,{  10,  10,  2, acFont8_1_189 } /* code 189 */
	,{  10,  10,  2, acFont8_1_190 } /* code 190 */
	,{   6,   6,  1, acFont8_1_191 } /* code 191 */
	,{   8,   8,  1, acFont8_1_192 } /* code 192 */
	,{   8,   8,  1, acFont8_1_193 } /* code 193 */
	,{   8,   8,  1, acFont8_1_194 } /* code 194 */
	,{   8,   8,  1, acFont8_1_195 } /* code 195 */
	,{   8,   8,  1, acFont8_1_196 } /* code 196 */
	,{   8,   8,  1, acFont8_1_197 } /* code 197 */
	,{  11,  11,  2, acFont8_1_198 } /* code 198 */
	,{   7,   7,  1, acFont8_1_199 } /* code 199 */
	,{   6,   6,  1, acFont8_1_200 } /* code 200 */
	,{   6,   6,  1, acFont8_1_201 } /* code 201 */
	,{   6,   6,  1, acFont8_1_202 } /* code 202 */
	,{   6,   6,  1, acFont8_1_203 } /* code 203 */
	,{   2,   2,  1, acFont8_1_204 } /* code 204 */
	,{   2,   2,  1, acFont8_1_205 } /* code 205 */
	,{   2,   2,  1, acFont8_1_206 } /* code 206 */
	,{   2,   2,  1, acFont8_1_207 } /* code 207 */
	,{   8,   8,  1, acFont8_1_208 } /* code 208 */
	,{   7,   7,  1, acFont8_1_209 } /* code 209 */
	,{   8,   8,  1, acFont8_1_210 } /* code 210 */
	,{   8,   8,  1, acFont8_1_211 } /* code 211 */
	,{   8,   8,  1, acFont8_1_212 } /* code 212 */
	,{   8,   8,  1, acFont8_1_213 } /* code 213 */
	,{   8,   8,  1, acFont8_1_214 } /* code 214 */
	,{   6,   6,  1, acFont8_1_215 } /* code 215 */
	,{   8,   8,  1, acFont8_1_216 } /* code 216 */
	,{   7,   7,  1, acFont8_1_217 } /* code 217 */
	,{   7,   7,  1, acFont8_1_218 } /* code 218 */
	,{   7,   7,  1, acFont8_1_219 } /* code 219 */
	,{   7,   7,  1, acFont8_1_220 } /* code 220 */
	,{   8,   8,  1, acFont8_1_221 } /* code 221 */
	,{   7,   7,  1, acFont8_1_222 } /* code 222 */
	,{   7,   7,  1, acFont8_1_223 } /* code 223 */
	,{   6,   6,  1, acFont8_1_224 } /* code 224 */
	,{   6,   6,  1, acFont8_1_225 } /* code 225 */
	,{   6,   6,  1, acFont8_1_226 } /* code 226 */
	,{   6,   6,  1, acFont8_1_227 } /* code 227 */
	,{   6,   6,  1, acFont8_1_228 } /* code 228 */
	,{   6,   6,  1, acFont8_1_229 } /* code 229 */
	,{  10,  10,  2, acFont8_1_230 } /* code 230 */
	,{   6,   6,  1, acFont8_1_231 } /* code 231 */
	,{   6,   6,  1, acFont8_1_232 } /* code 232 */
	,{   6,   6,  1, acFont8_1_233 } /* code 233 */
	,{   6,   6,  1, acFont8_1_234 } /* code 234 */
	,{   6,   6,  1, acFont8_1_235 } /* code 235 */
	,{   2,   2,  1, acFont8_1_236 } /* code 236 */
	,{   2,   2,  1, acFont8_1_237 } /* code 237 */
	,{   2,   2,  1, acFont8_1_238 } /* code 238 */
	,{   2,   2,  1, acFont8_1_239 } /* code 239 */
	,{   6,   6,  1, acFont8_1_240 } /* code 240 */
	,{   6,   6,  1, acFont8_1_241 } /* code 241 */
	,{   6,   6,  1, acFont8_1_242 } /* code 242 */
	,{   6,   6,  1, acFont8_1_243 } /* code 243 */
	,{   6,   6,  1, acFont8_1_244 } /* code 244 */
	,{   6,   6,  1, acFont8_1_245 } /* code 245 */
	,{   6,   6,  1, acFont8_1_246 } /* code 246 */
	,{   6,   6,  1, acFont8_1_247 } /* code 247 */
	,{   6,   6,  1, acFont8_1_248 } /* code 248 */
	,{   6,   6,  1, acFont8_1_249 } /* code 249 */
	,{   6,   6,  1, acFont8_1_250 } /* code 250 */
	,{   6,   6,  1, acFont8_1_251 } /* code 251 */
	,{   6,   6,  1, acFont8_1_252 } /* code 252 */
	,{   6,   6,  1, acFont8_1_253 } /* code 253 */
	,{   6,   6,  1, acFont8_1_254 } /* code 254 */
	,{   6,   6,  1, acFont8_1_255 } /* code 255 */
};

const FontProp GUI_Font8_1 {
	8, /* height of font  */
	8, /* space of font y */
	7,
	5,
	7,
	160, /* first character */
	255, /* last character */
	Font8_1_CharInfo, /* address of first character */
	&GUI_Font8_ASCII /* pointer to next GUI_FONT_PROP */
};

const FontProp GUI_Font8_2 {
	8, /* height of font  */
	8 * 2, /* space of font y */
	7,
	5,
	7,
	160, /* first character */
	255, /* last character */
	Font8_1_CharInfo, /* address of first character */
	&GUI_Font8_ASCII /* pointer to next GUI_FONT_PROP */
};
