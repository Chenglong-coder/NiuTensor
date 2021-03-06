/* NiuTrans.Tensor - an open-source tensor library
 * Copyright (C) 2020, Natural Language Processing Lab, Northeastern University.
 * All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * $Created by: XIAO Tong (xiaotong@mail.neu.edu.cn) 2018-07-31
 * $Modified by: HU Chi (huchinlp@gmail.com) 2020-04
 */

#include <cmath>

#include "T2TFNN.h"
#include "T2TUtility.h"
#include "T2TEmbedding.h"
#include "../../../tensor/core/CHeader.h"
#include "../../../tensor/function/FHeader.h"

namespace transformer
{

/* constructor */
T2TFNN::T2TFNN()
{
    inSize = -1;
    outSize = -1;
    hSize = -1;
}

/* de-constructor */
T2TFNN::~T2TFNN()
{
}

/*
initialize the model
>> argc - number of arguments
>> argv - list of pointers to the arguments
>> config - configurations of the model
*/
void T2TFNN::InitModel(T2TConfig& config)
{
    devID = config.devID;

    inSize = config.modelSize;
    outSize = config.modelSize;
    hSize = config.fnnHiddenSize;
    dropoutP = config.fnnDropout;

    InitTensor2D(&w1, inSize, hSize, X_FLOAT, devID);
    InitTensor1D(&b1, hSize, X_FLOAT, devID);

    InitTensor2D(&w2, hSize, outSize, X_FLOAT, devID);
    InitTensor1D(&b2, outSize, X_FLOAT, devID);

    float scale = 1.0F;
    _SetDataFanInOut(&w1, scale);
    _SetDataFanInOut(&w2, scale);

    b1.SetZeroAll();
    b2.SetZeroAll();
}

/*
make the network
y = max(0, x * w1 + b1) * w2 + b2
>> input - the input tensor
>> return - the output tensor
*/
XTensor T2TFNN::Make(XTensor& input, bool isTraining)
{
    XTensor t1;

    /* t1 = max(0, x * w1 + b1) */
    t1 = Rectify(MulAndShift(input, w1, b1));

    if (isTraining && dropoutP > 0)
        t1 = Dropout(t1, dropoutP);

    /* result = t1 * w2 + b2 */
    return MulAndShift(t1, w2, b2);
}

}