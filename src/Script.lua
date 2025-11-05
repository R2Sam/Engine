function Update(deltaT)
    if HasTransform(Entity) then
        transform = GetTransform(Entity)

        if transform.position.x >= 1280 then
            transform.velocity.x = -transform.velocity.x;
        elseif transform.position.x <= 0 then
            transform.velocity.x = -transform.velocity.x;
        end

        if transform.position.y >= 720 then
            transform.velocity.y = -transform.velocity.y;
        elseif transform.position.y <= 0 then
            transform.velocity.y = -transform.velocity.y;
        end

        speed = math.sqrt((transform.velocity.x)^2 + (transform.velocity.y)^2)

        if speed < 10000 then
            transform.velocity.x = transform.velocity.x * 1.001
            transform.velocity.y = transform.velocity.y * 1.001
        end

        transform.position.x = transform.position.x + transform.velocity.x * deltaT
        transform.position.y = transform.position.y + transform.velocity.y * deltaT

    end
end