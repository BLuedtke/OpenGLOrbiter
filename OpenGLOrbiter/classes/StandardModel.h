//  Modified by: Bernhard Luedtke
//  Author of the original Class:
    //  Created by Philipp Lensing on 19.09.16.


#ifndef StandardModel_hpp
#define StandardModel_hpp

#include <stdio.h>
#include "camera.h"
#include "matrix.h"
#include "StandardShader.h"

class StandardModel
{
public:
    StandardModel();
    virtual ~StandardModel();
    virtual void draw(const BaseCamera& Cam);
	const Matrix& transform();
	void transform(Matrix pMatrix);
	virtual void setShader(std::unique_ptr<StandardShader> uPShader);

protected:
	std::unique_ptr<StandardShader> uShader;
	Matrix uTransform;
};


#endif /* StandardModel_hpp */
